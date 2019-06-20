/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   CC1101配置C文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */

#include "drv_CC1101.h"
#include "drv_delay.h"

//10, 7, 5, 0, -5, -10, -15, -20, dbm output power, 0x12 == -30dbm
const uint8_t PaTabel[] = {0xc0, 0xC8, 0x84, 0x60, 0x68, 0x34, 0x1D, 0x0E};
static const uint8_t CC1101InitData[23][2] = {
    {CC1101_IOCFG2, 0x01},   //RX FIFO is filled/溢出/packet end
    {CC1101_IOCFG0, 0x06},   //sync word has been sent / received
    {CC1101_FIFOTHR, 0x47},  //Bytes in TX FIFO:33    Bytes in RX FIFO:32
    {CC1101_PKTCTRL0, 0x05}, //CRC TX&RX enable   Variable packet length mode.
    {CC1101_CHANNR, 0x96},   //channel number:150  430M
    {CC1101_FSCTRL1, 0x06},  //f_IF = 152343.75
    {CC1101_FREQ2, 0x0F},    //f_carrier_bace = 399.99M
    {CC1101_FREQ1, 0x62},    //
    {CC1101_FREQ0, 0x76},    //
    {CC1101_MDMCFG4, 0xF6},  //CH_bandwidth=26M/(8*(4+3)*(2^3)) = 50k
    {CC1101_MDMCFG3, 0x43},  //Rate = 2k
    {CC1101_MDMCFG2, 0x13},  //
    {CC1101_DEVIATN, 0x15},  //载波偏差 5.157471k
    {CC1101_MCSM1, 0x3F},    //
    {CC1101_MCSM0, 0x18},    //IDLE to RX or Tx 自动校准
    {CC1101_FOCCFG, 0x16},
    {CC1101_WORCTRL, 0xFB},
    {CC1101_FSCAL3, 0xE9},
    {CC1101_FSCAL2, 0x2A},
    {CC1101_FSCAL1, 0x00},
    {CC1101_FSCAL0, 0x1F},
    {CC1101_TEST2, 0x81},
    {CC1101_TEST1, 0x35},
};

/**
  * @brief :CC1101写命令
  * @param :
  *			@Command：命令
  * @note  :无
  * @retval:无
  */
void CC1101_Write_Cmd(uint8_t Command)
{
    CC1101_SET_CSN_LOW(); //SPI片选，本工程中该函数都是用作SPI片选

    drv_spi_read_write_byte(Command); //写命令

    CC1101_SET_CSN_HIGH(); //SPI取消片选，本工程中该函数都是用作取消SPI片选
}

/**
  * @brief :CC1101写寄存器
  * @param :
  *			@Addr：地址
  *			@WriteValue：写入的数据字节
  * @note  :无
  * @retval:无
  */
void CC1101_Write_Reg(uint8_t Addr, uint8_t WriteValue)
{
    CC1101_SET_CSN_LOW();

    drv_spi_read_write_byte(Addr);       //写地址
    drv_spi_read_write_byte(WriteValue); //写数据

    CC1101_SET_CSN_HIGH();
}

/**
  * @brief :CC1101连续写寄存器
  * @param :
  *			@Addr：地址
  *			@pWriteBuff：写入的数据串首地址
  *			@WriteSize：写入的数据个数
  * @note  :无
  * @retval:无
  */
void CC1101_Write_Multi_Reg(uint8_t Addr, uint8_t *pWriteBuff, uint8_t WriteSize)
{
    uint8_t i;

    CC1101_SET_CSN_LOW();

    drv_spi_read_write_byte(Addr | WRITE_BURST); //连续写命令 及首地址
    for (i = 0; i < WriteSize; i++)
    {
        drv_spi_read_write_byte(*(pWriteBuff + i)); //连续写入数据
    }

    CC1101_SET_CSN_HIGH();
}

/**
  * @brief :CC1101读寄存器
  * @param :
  *			@Addr：地址
  * @note  :无
  * @retval:寄存器值
  */
uint8_t CC1101_Read_Reg(uint8_t Addr)
{
    uint8_t l_RegValue = 0;

    CC1101_SET_CSN_LOW();

    drv_spi_read_write_byte(Addr | READ_SINGLE); //单独读命令 及地址
    l_RegValue = drv_spi_read_write_byte(0xFF);  //读取寄存器

    CC1101_SET_CSN_HIGH();

    return l_RegValue;
}

/**
  * @brief :CC1101读一个寄存器状态
  * @param :
  *			@Addr：地址
  * @note  :无
  * @retval:寄存器状态
  */
uint8_t CC1101_Read_Status(uint8_t Addr)
{
    uint8_t l_RegStatus = 0;

    CC1101_SET_CSN_LOW();

    drv_spi_read_write_byte(Addr | READ_BURST);  //连续读命令 及地址
    l_RegStatus = drv_spi_read_write_byte(0xFF); //读取状态

    CC1101_SET_CSN_HIGH();

    return l_RegStatus;
}

/**
  * @brief :CC1101连续读寄存器
  * @param :
  *			@Addr：地址
  *			@pReadBuff：读取数据存放首地址
  *			@ReadSize：读取数据的个数
  * @note  :无
  * @retval:无
  */
void CC1101_Read_Multi_Reg(uint8_t Addr, uint8_t *pReadBuff, uint8_t ReadSize)
{
    uint8_t i = 0, j = 0;

    CC1101_SET_CSN_LOW();

    drv_spi_read_write_byte(Addr | READ_BURST); //连续读命令 及首地址
    for (i = 0; i < ReadSize; i++)
    {
        for (j = 0; j < 20; j++)
            ;
        *(pReadBuff + i) = drv_spi_read_write_byte(0xFF); //连续读取数据
    }

    CC1101_SET_CSN_HIGH();
}

/**
  * @brief :CC1101发送接收模式设置
  * @param :
  *			@Mode：TX_MODE，发送模式 RX_MODE，接收模式
  * @note  :无
  * @retval:寄存器状态
  */
void CC1101_Set_Mode(CC1101_ModeType Mode)
{
    if (Mode == TX_MODE) //发送模式
    {
        CC1101_Write_Reg(CC1101_IOCFG0, 0x46);
        CC1101_Write_Cmd(CC1101_STX);
    }
    else if (Mode == RX_MODE) //接收模式
    {
        CC1101_Write_Reg(CC1101_IOCFG0, 0x46);
        CC1101_Write_Cmd(CC1101_SRX);
    }

    while (0 != CC1101_GET_GDO0_STATUS())
        ; //等待发送 或 接收开始
}
/**
  * @brief :CC1101发送接收模式设置
  * @param :
  *			@Mode RX_MODE，接收模式
  * @note  :无
  * @retval:寄存器状态
  */
void CC1101_Set_RX_Mode(void)
{
    CC1101_Write_Reg(CC1101_IOCFG0, 0x46);
    CC1101_Write_Cmd(CC1101_SRX);
}

/**
  * @brief :CC1101进入空闲模式
  * @param :无
  * @note  :无
  * @retval:无
  */
void CC1101_Set_Idle_Mode(void)
{
    CC1101_Write_Cmd(CC1101_SIDLE);
}

/**
  * @brief :CC1101初始化WOR功能
  * @param :无
  * @note  :无
  * @retval:无
  */
void C1101_WOR_Init(void)
{
    CC1101_Write_Reg(CC1101_MCSM0, 0x18);
    CC1101_Write_Reg(CC1101_WORCTRL, 0x78);
    CC1101_Write_Reg(CC1101_MCSM2, 0x00);
    CC1101_Write_Reg(CC1101_WOREVT1, 0x8C);
    CC1101_Write_Reg(CC1101_WOREVT0, 0xA0);
    CC1101_Write_Cmd(CC1101_SWORRST); //写入WOR命令
}

/**
  * @brief :CC1101设置地址
  * @param :
  *			@Address：设置的设备地址值
  *			@AddressMode：地址检测模式
  * @note  :无
  * @retval:无
  */
void CC1101_Set_Address(uint8_t Address, CC1101_AddrModeType AddressMode)
{
    uint8_t btmp = 0;

    btmp = CC1101_Read_Reg(CC1101_PKTCTRL1) & ~0x03; //读取CC1101_PKTCTRL1寄存器初始值
    CC1101_Write_Reg(CC1101_ADDR, Address);          //设置设备地址

    if (AddressMode == BROAD_ALL)
    {
    } //不检测地址
    else if (AddressMode == BROAD_NO)
    {
        btmp |= 0x01; //检测地址 但是不带广播
    }
    else if (AddressMode == BROAD_0)
    {
        btmp |= 0x02; //0x00为广播
    }
    else if (AddressMode == BROAD_0AND255)
    {
        btmp |= 0x03; //0x00 0xFF为广播
    }

    CC1101_Write_Reg(CC1101_PKTCTRL1, btmp); //写入地址模式
}

/**
  * @brief :CC1101设置同步字段
  * @param :无
  * @note  :无
  * @retval:无
  */
void CC1101_Set_Sync(uint16_t Sync)
{
    CC1101_Write_Reg(CC1101_SYNC1, 0xFF & (Sync >> 8));
    CC1101_Write_Reg(CC1101_SYNC0, 0xFF & Sync); //写入同步字段 16Bit
}

/**
  * @brief :CC1101清空发送缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void CC1101_Clear_TxBuffer(void)
{
    CC1101_Set_Idle_Mode();        //首先进入IDLE模式
    CC1101_Write_Cmd(CC1101_SFTX); //写入清发送缓冲区命令
}

/**
  * @brief :CC1101清空接收缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void CC1101_Clear_RxBuffer(void)
{
    CC1101_Set_Idle_Mode();        //首先进入IDLE模式
    CC1101_Write_Cmd(CC1101_SFRX); //写入清接收缓冲区命令
}

/**
  * @brief :CC1101发送数据包
  * @param :
  *			@pTxBuff：发送数据缓冲区
  *			@TxSize：发送数据长度
  *			@DataMode：数据模式
  * @note  :无
  * @retval:无
  */
void CC1101_Tx_Packet(uint8_t *pTxBuff, uint8_t TxSize, CC1101_TxDataModeType DataMode)
{
    uint8_t Address;
    uint16_t l_RxWaitTimeout = 0;

    if (DataMode == BROADCAST)
    {
        Address = 0;
    }
    else if (DataMode == ADDRESS_CHECK)
    {
        Address = CC1101_Read_Reg(CC1101_ADDR);
    }

    CC1101_Clear_TxBuffer();

    if ((CC1101_Read_Reg(CC1101_PKTCTRL1) & 0x03) != 0)
    {
        CC1101_Write_Reg(CC1101_TXFIFO, TxSize + 1);
        CC1101_Write_Reg(CC1101_TXFIFO, Address); //写入长度和地址 由于多一个字节地址此时长度应该加1
    }
    else
    {
        CC1101_Write_Reg(CC1101_TXFIFO, TxSize); //只写长度 不带地址
    }

    CC1101_Write_Multi_Reg(CC1101_TXFIFO, pTxBuff, TxSize); //写入数据
    CC1101_Set_Mode(TX_MODE);                               //发送模式

    while (0 == CC1101_GET_GDO0_STATUS()) //等待发送完成
    {
        drv_delay_ms(1);
        if (1000 == l_RxWaitTimeout++)
        {
            l_RxWaitTimeout = 0;
            CC1101_Init();
            break;
        }
    }
}

/**
  * @brief :CC1101读取接收到的字节数
  * @param :无
  * @note  :无
  * @retval:接收到的数据个数
  */
uint8_t CC1101_Get_RxCounter(void)
{
    return (CC1101_Read_Status(CC1101_RXBYTES) & BYTES_IN_RXFIFO);
}

/**
  * @brief :CC1101接收数据包
  * @param :
  *			@RxBuff：发送数据缓冲区
  * @note  :无
  * @retval：接收到的字节数，0表示无数据
  */
uint8_t CC1101_Rx_Packet(uint8_t *RxBuff)
{
    uint8_t l_PktLen = 0;
    uint8_t l_Status[2] = {0};
    uint16_t l_RxWaitTimeout = 0;
    uint8_t temp[255];

    while (0 == CC1101_GET_GDO0_STATUS()) //等待接收完成
    {
        drv_delay_ms(1);
        if (3000 == l_RxWaitTimeout++)
        {
            l_RxWaitTimeout = 0;
            CC1101_Init();
            break;
        }
    }

    if (0 != CC1101_Get_RxCounter())
    {
        l_PktLen = CC1101_Read_Reg(CC1101_RXFIFO); // 获取长度信息

        if ((CC1101_Read_Reg(CC1101_PKTCTRL1) & 0x03) != 0)
        {
            CC1101_Read_Reg(CC1101_RXFIFO); //如果数据包中包含地址信息 ，则读取地址信息
        }
        if (l_PktLen == 0)
        {
            return 0; //无数据
        }
        else
        {
            l_PktLen--; //减去一个地址字节
        }
        CC1101_Read_Multi_Reg(CC1101_RXFIFO, RxBuff, l_PktLen); //读取数据
        CC1101_Read_Multi_Reg(CC1101_RXFIFO, l_Status, 2);      //读取数据包最后两个额外字节，后一个为CRC标志位

        while (0 != CC1101_GET_GDO2_STATUS())
        {
            l_Status[0] = CC1101_Get_RxCounter();
            if (l_Status[0] == 0)
                break;
            /* code */
            CC1101_Read_Multi_Reg(CC1101_RXFIFO, temp, l_Status[0] < 250 ? l_Status[0] : 250); //读取数据
        }
        // CC1101_Clear_RxBuffer();
        if (l_Status[1] & CRC_OK)
        {
            return l_PktLen;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

/**
  * @brief :CC1101复位
  * @param :无
  * @note  :无
  * @retval:无
  */
void CC1101_Reset(void)
{
    CC1101_SET_CSN_HIGH();
    CC1101_SET_CSN_LOW();
    CC1101_SET_CSN_HIGH();
    drv_delay_us(40);
    CC1101_Write_Cmd(CC1101_SRES);
}

/**
  * @brief :CC1101引脚初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
static void CC1101_Gpio_Init(void)
{
    GPIO_InitTypeDef GpioInitStructer;

    //使能口线时钟
    RCC_APB2PeriphClockCmd(CC1101_GDO0_GPIO_CLK | CC1101_GDO2_GPIO_CLK, ENABLE); //打开端口时钟

    //GDO0 GDO2配置为上拉输入
    GpioInitStructer.GPIO_Speed = GPIO_Speed_2MHz;
    GpioInitStructer.GPIO_Mode = GPIO_Mode_IPU;
    GpioInitStructer.GPIO_Pin = CC1101_GDO0_GPIO_PIN;
    GPIO_Init(CC1101_GDO0_GPIO_PORT, &GpioInitStructer);

    GpioInitStructer.GPIO_Pin = CC1101_GDO2_GPIO_PIN;
    GPIO_Init(CC1101_GDO2_GPIO_PORT, &GpioInitStructer);
}

/**
  * @brief :CC1101初始化
  * @param :无
  * @note  :无
  * @retval:无
  */
void CC1101_Init(void)
{
    uint8_t i = 0;

    CC1101_Gpio_Init(); //引脚初始化
    CC1101_Reset();     //模块复位

    for (i = 0; i < 22; i++)
    {
        CC1101_Write_Reg(CC1101InitData[i][0], CC1101InitData[i][1]); //写入配置参数
    }
    CC1101_Set_Address(0x05, BROAD_0AND255); //写入设备地址 和地址模式
    CC1101_Set_Sync(0x8799);                 //写入同步字段
    CC1101_Write_Reg(CC1101_MDMCFG1, 0x72);  //调制解调器配置

    CC1101_Write_Multi_Reg(CC1101_PATABLE, (uint8_t *)PaTabel, 8);
}
