#include <stm32_core.h>

#define STM32_LL_I2C_TIMEOUT        200

#define IS_CR1_FLAG_SET(a,flag)     ((a->Instance->CR1 & flag) == flag)
#define CR1_FLAG_SET(a,flag)        (a->Instance->CR1 |= flag)
#define CR1_FLAG_CLR(a,flag)        (a->Instance->CR1 &= ~flag)

#define DR_SET(a,byte)              (a->Instance->DR = byte)
#define DR_GET(a)                   (a->Instance->DR)

extern vatek_result stmi2c_set_speed(hstm32_i2c hi2c, int32_t speedkhz);
extern vatek_result stmi2c_start(hstm32_i2c hi2c, uint8_t devaddr,int32_t restart);		/* include write|write bit*/
extern vatek_result stmi2c_write(hstm32_i2c hi2c,const uint8_t* pbuf, int32_t len);
extern vatek_result stmi2c_read(hstm32_i2c hi2c, uint8_t* pbuf, int32_t len);
extern vatek_result stmi2c_stop(hstm32_i2c hi2c);

vatek_result stm32_core_create_i2c(I2C_HandleTypeDef* i2c,Pstm32_i2c* pi2cdrv)
{
    Pstm32_i2c newi2c = (Pstm32_i2c)malloc(sizeof(stm32_i2c));
    vatek_result nres = vatek_memfail;
    if(newi2c != NULL)
    {
        memset(newi2c,0,sizeof(stm32_i2c));
        newi2c->hi2c = i2c;
        newi2c->set_speed = stmi2c_set_speed;
        newi2c->start = stmi2c_start;
        newi2c->stop = stmi2c_stop;
        newi2c->write = stmi2c_write;
        newi2c->read = stmi2c_read;
        *pi2cdrv = newi2c;
        nres = vatek_success;
    }
    return nres;
}

vatek_result stm32_core_free_i2c(Pstm32_i2c pi2cdrv)
{
    HAL_I2C_DeInit((I2C_HandleTypeDef*)pi2cdrv->hi2c);
    free(pi2cdrv);
    return vatek_success;
}


extern vatek_result ll_i2c_wait_flag(I2C_HandleTypeDef* hi2c,uint32_t flag,FlagStatus status);
extern vatek_result ll_i2c_wait_flag_clr(I2C_HandleTypeDef* hi2c,uint32_t flag);
extern vatek_result ll_i2c_wait_flag_set(I2C_HandleTypeDef* hi2c,uint32_t flag);
extern vatek_result ll_i2c_send_start(I2C_HandleTypeDef* hi2c);
extern vatek_result ll_i2c_check_busy(I2C_HandleTypeDef* hi2c,uint8_t lock);

extern vatek_result ll_i2c_set_device_address(hstm32_i2c hi2c,uint8_t devaddr,int32_t nwrite);

vatek_result stmi2c_start(hstm32_i2c hi2c, uint8_t devaddr,int32_t restart)
{
    vatek_result nres = vatek_unknown;
    int32_t iswrite = !(devaddr & 1);
    uint8_t addr = devaddr & 0xFE;
    
	uint8_t is_lock_i2c = (restart)? 0:1;
    I2C_HandleTypeDef* si2c = (I2C_HandleTypeDef*)hi2c;
    
    nres = ll_i2c_check_busy(si2c, is_lock_i2c);
    if(is_vatek_success(nres))
    {
        if(!IS_CR1_FLAG_SET(si2c,I2C_CR1_PE))
            CR1_FLAG_SET(si2c,I2C_CR1_PE);
        
        CR1_FLAG_CLR(si2c,I2C_CR1_POS);
        nres = ll_i2c_send_start(si2c);
    }
    
    if(is_vatek_success(nres))
    {
        nres = ll_i2c_set_device_address(hi2c,addr,iswrite);
    }

    return nres;    
}

vatek_result stmi2c_set_speed(hstm32_i2c hi2c, int32_t speedkhz)
{
    return vatek_success;
}

vatek_result ll_i2c_set_device_address(hstm32_i2c hi2c,uint8_t devaddr,int32_t nwrite)
{
    vatek_result nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_SB);
    I2C_HandleTypeDef* si2c = (I2C_HandleTypeDef*)hi2c;
    if(is_vatek_success(nres))
    {
        if(nwrite)DR_SET(si2c,I2C_7BIT_ADD_WRITE(devaddr));
        else DR_SET(si2c,I2C_7BIT_ADD_READ(devaddr));
        
        nres = ll_i2c_wait_flag_set(si2c,I2C_FLAG_ADDR);
    }
    
    if(__HAL_I2C_GET_FLAG(si2c, I2C_FLAG_AF) == SET)
    {
        _S_ERR("I2C_FLAG_AF i2c stop");/* Generate Stop */
    }else{
		si2c->State = HAL_I2C_STATE_READY;
        /* Clear ADDR flag */
        __HAL_I2C_CLEAR_ADDRFLAG(si2c); 	
		
	}
    return nres;    
}

vatek_result stmi2c_write(hstm32_i2c hi2c,const uint8_t* pbuf, int32_t len)
{   
	vatek_result nres  = vatek_success;
    I2C_HandleTypeDef* si2c = (I2C_HandleTypeDef*)hi2c;
    if(is_vatek_success(nres))
    {     
        while(len && is_vatek_success(nres))
        {
            nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_TXE);
            if(is_vatek_success(nres))
            {                               
                DR_SET(si2c,*pbuf++);
                len--;
                nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_BTF);
            }else break;
        }
        
        if(is_vatek_success(nres))nres = ll_i2c_wait_flag_set(si2c,I2C_FLAG_TXE);
        else _S_LOG("stm32_ll_i2c_write fail : %d",nres);
    }
    return nres;    
}

vatek_result stmi2c_read(hstm32_i2c hi2c, uint8_t* pbuf, int32_t len)
{
	vatek_result nres = vatek_success;
    I2C_HandleTypeDef* si2c = (I2C_HandleTypeDef*)hi2c;
    if(is_vatek_success(nres))
    {    
        if(len == 1)
        {
            CR1_FLAG_CLR(si2c,I2C_CR1_ACK);
        }else if(len == 2)
        {
            CR1_FLAG_CLR(si2c,I2C_CR1_ACK);
            CR1_FLAG_SET(si2c,I2C_CR1_POS);
        }
		else CR1_FLAG_SET(si2c,I2C_CR1_ACK);
        __HAL_I2C_CLEAR_ADDRFLAG(si2c);
        
		while(len && is_vatek_success(nres))
        {
			if(len <= 3){
				if(len == 1)
				{
					nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_RXNE);
					if(is_vatek_success(nres))
					{
						*pbuf++ = DR_GET(si2c);
						len--;
					}
				}else if(len == 2){
					nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_BTF);
					if(is_vatek_success(nres))
					{
						*pbuf++ = DR_GET(si2c);
						len--;
						*pbuf++ = DR_GET(si2c);
						len--;
					}
				}else{
					nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_BTF);
					CR1_FLAG_CLR(si2c,I2C_CR1_ACK);
										
					if(is_vatek_success(nres))
					{
						*pbuf++ = DR_GET(si2c);
						len--;
						nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_BTF);
						if(is_vatek_success(nres)){
							*pbuf++ = DR_GET(si2c);
							len--;
							*pbuf++ = DR_GET(si2c);
							len--;
						}
					}
				}
			}else{
				
				nres = ll_i2c_wait_flag_set(hi2c,I2C_FLAG_RXNE);
				if(!is_vatek_success(nres))return nres;
				else
				{
					*pbuf++ = DR_GET(si2c);
					 len--;
					if(__HAL_I2C_GET_FLAG(si2c, I2C_FLAG_BTF) == SET){
						*pbuf++ = DR_GET(si2c);
						len--;
					}
				}			
			}    
        }     
    }
    
    if(!is_vatek_success(nres))_S_LOG("stm32_ll_i2c_read fail : %d",nres);
    return nres;        
}

 vatek_result stmi2c_stop(hstm32_i2c hi2c)
{
    I2C_HandleTypeDef* si2c = (I2C_HandleTypeDef*)hi2c;
	si2c->Instance->CR1 |= I2C_CR1_STOP;
    si2c->State = HAL_I2C_STATE_READY;
    si2c->Lock = HAL_UNLOCKED;
    return vatek_success;    
}

vatek_result ll_i2c_wait_flag(I2C_HandleTypeDef* hi2c,uint32_t flag,FlagStatus status)
{
    uint32_t ntick = HAL_GetTick();
    
    while((__HAL_I2C_GET_FLAG(hi2c,flag) ? SET : RESET) != status)
    {
        if((HAL_GetTick() - ntick) > STM32_LL_I2C_TIMEOUT)return vatek_timeout;
    }
    
    return vatek_success;
}

vatek_result ll_i2c_wait_flag_clr(I2C_HandleTypeDef* hi2c,uint32_t flag)
{
    return ll_i2c_wait_flag(hi2c,flag,RESET);
}

vatek_result ll_i2c_wait_flag_set(I2C_HandleTypeDef* hi2c,uint32_t flag)
{
    return ll_i2c_wait_flag(hi2c,flag,SET);
}

vatek_result ll_i2c_send_start(I2C_HandleTypeDef* hi2c)
{
    CR1_FLAG_SET(hi2c,I2C_CR1_START);
    return ll_i2c_wait_flag_set(hi2c,I2C_FLAG_SB); 
}

vatek_result ll_i2c_check_busy(I2C_HandleTypeDef* hi2c, uint8_t lock)
{
    vatek_result nres = vatek_badstatus;
    
    if(hi2c->Lock == HAL_LOCKED){
		if(lock == 0) return vatek_success;		//i2c restart process
		else return vatek_badstatus;
	}
	
    nres = ll_i2c_wait_flag_clr(hi2c,I2C_FLAG_BUSY);
    if(is_vatek_success(nres))if(lock)hi2c->Lock = HAL_LOCKED;        
    return nres;
}

