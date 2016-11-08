// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "prf_defs.h"

#if (BUT_PROFILE_CFG)


#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>


#include "rom/gpio.h"
#include "app_button.h"
#include "bt_trace.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "thread.h"

#include "allocator.h"
#include "button_pro.h"
#include "app_button_int.h"

static const tBT_PRF_SYS_REG but_prf_reg =
{
    ble_but_prf_hdl_event,
    ble_but_prf_disable
};



/*******************************************************************************
**
** Function         ble_but_prf_hdl_event
**
** Description      button profile main event handling function.
**
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN ble_but_prf_hdl_event(prf_hdr_evt_t *msg_data)
{
	LOG_ERROR("###################ble_but_prf_hdl_event#####################################\n");
	
	UINT16 connid = 0;
	switch(msg_data->event)
	{
		case BLE_BUT_CREATE_DB_REQ_EVT: 
			Button_CreateService();
			break;
		case BLE_BUT_VAL_SEND_CFM_EVT:
			break;
		case BLE_BUT_SET_CHAR_VAL_REQ_EVT:
			button_msg_notify(msg_data->len,msg_data->data);
			break;
		case BLE_BUT_ENABLE_REQ_EVT:
			button_init(NULL);
			break;
		case BLE_BUT_DISABLE_IND_EVT:
			button_disable(connid);
			break;
		case BLE_BUT_CHAR_WRITE_IND_EVT:
			
			break;
		case BLE_BUT_ERROR_IND_EVT:
			break;
		default:
			break;
	}
}


/*******************************************************************************
**
** Function         ble_but_prf_disable
**
** Description      This function is called to disable the button profile modlue
**
** Parameters       None.
**
** Returns          None
**
*******************************************************************************/
void ble_but_prf_disable(void)
{
    prf_hdr_evt_t  *p_buf;
	LOG_ERROR("ble_but_prf_disable\n");
	
    if (bt_prf_sys_is_register(PRF_ID_BUT_LE) == FALSE)
    {
        APPL_TRACE_WARNING("button profile Module not enabled/already disabled\n");
        return;
    }

    if ((p_buf = (prf_hdr_evt_t *) GKI_getbuf(sizeof(prf_hdr_evt_t))) != NULL)
    {
        p_buf->event = BLE_BUT_DISABLE_IND_EVT;
        bta_sys_sendmsg(p_buf);
    }
    bta_sys_deregister(PRF_ID_BUT_LE);

}

void ble_but_prf_enable(void)
{
	bt_prf_sys_register(PRF_ID_BUT_LE,&but_prf_reg);
}

void ble_but_create_svc(void)
{
	prf_hdr_evt_t *p_msg;

	LOG_ERROR("ble_but_create_svc\n"); 		//todo
    if ((p_msg = (prf_hdr_evt_t *) GKI_getbuf(sizeof(prf_hdr_evt_t))) != NULL)
    {
        memset(p_msg, 0, sizeof(prf_hdr_evt_t));

        p_msg->event = BLE_BUT_ENABLE_REQ_EVT;
			
        bt_prf_sys_sendmsg(p_msg);
    }
}






#endif		///BUT_PROFILE_CFG

#if 0

#define GPIO_INUM 8
#define TABLE_ELEMENT_CNT(table) ((sizeof(table))/(sizeof(table[0])));
app_key_env key_press;

uint8_t gpio_test_table[]={0,2,4,5,12,13,14,15,16,17,18,19,20,21,22,23,25,26,27,33,34,35,36,37,38,39};

struct gpio_test_info{
	uint8_t *gpio_test_table;
	uint8_t gpio_test_num;
	void *time_s;
};

static void gpio_irq_init(uint64_t gpio_num);

void gpio_check_register(enum_gpio_num_t gpio_num)
{
    if(gpio_num>=GPIO_PIN_COUNT||0==GPIO_PIN_MUX_REG[gpio_num]){
        ets_printf("io_num=%d not exits\n",gpio_num);
		return;
	}
    LOG_ERROR("---------gpio_num %d reg----------\n",gpio_num);
LOG_ERROR("GPIO_IOMUX_%d=0x%08x\n",gpio_num,READ_PERI_REG(GPIO_PIN_MUX_REG[gpio_num]));
   LOG_ERROR("GPIO_PIN%d_ADDR=0x%08x\n",gpio_num,READ_PERI_REG(GPIO_PIN_ADDR(gpio_num)));
	LOG_ERROR("GPIO_OUT_REG=0x%08x\n",READ_PERI_REG(GPIO_OUT_REG));
	LOG_ERROR("GPIO_OUT1_REG=0x%08x\n",READ_PERI_REG(GPIO_OUT1_REG));
	LOG_ERROR("GPIO_ENABLE_REG=0x%08x\n",READ_PERI_REG(GPIO_ENABLE_REG));
	LOG_ERROR("GPIO_ENABLE1_REG=0x%08x\n",READ_PERI_REG(GPIO_ENABLE1_REG));
	LOG_ERROR("GPIO_IN_REG=0x%08x\n",READ_PERI_REG(GPIO_IN_REG));
	LOG_ERROR("GPIO_IN1_REG=0x%08x\n",READ_PERI_REG(GPIO_IN1_REG));
	LOG_ERROR("GPIO_STATUS_REG=0x%08x\n",READ_PERI_REG(GPIO_STATUS_REG));
	LOG_ERROR("GPIO_STATUS1_REG=0x%08x\n",READ_PERI_REG(GPIO_STATUS1_REG));
}

void t1_callback(void *arg)
{
	 static uint8_t level=0;
	static uint8_t cnt=0;
	uint8_t err_flag=0;
	struct gpio_test_info *gpio_test=(struct gpio_test_info*)(arg);
	uint8_t i=0;
	while(1){
	gpio_check_register(35);
	vTaskDelay(2*1000);	
	/*
        level=~level;
        LOG_ERROR("Test cnt %u, level %u\n",cnt+1,level&0x01);
        for(i=0;i<gpio_test->gpio_test_num;i++){
            gpio_set_output_level(gpio_test->gpio_test_table[i],level&0x01);
        	if(gpio_get_input_level(gpio_test->gpio_test_table[i])!=(level&0x01))
        	{
        	    err_flag=1;
                LOG_ERROR("[ERR] GPIO%u set_level %u get_level %u\n",gpio_test->gpio_test_table[i],level&0x01,gpio_get_input_level(gpio_test->gpio_test_table[i]));
        	}
			else{
               LOG_ERROR("GPIO%u OK\n",gpio_test->gpio_test_table[i]);
			}
        }
        cnt++;
		if(err_flag==0){
          LOG_ERROR("cnt %u test ok\n",cnt);  
		}
		err_flag=0;
        if(cnt>=10){
           LOG_ERROR("Gpio input and output test end\n");
           vTaskDelete(NULL);*/
       // }
		
		//vTaskDelay(2*1000);
		
	}
}

 void app_button_init(void)
{
	uint64_t gpio_num = GPIO_Pin_27|GPIO_Pin_35|GPIO_Pin_34|GPIO_Pin_36|GPIO_Pin_39;
	// Reset environment
    memset(&key_press, 0, sizeof(key_press));	
	gpio_irq_init(gpio_num);
	static struct gpio_test_info gpio_test_infor; 
        LOG_ERROR("app_button_init.");
/*	TimerHandle_t t1=NULL;
   t1=xTimerCreate("t1_time",(1000/portTICK_PERIOD_MS),pdTRUE,&gpio_test_infor,t1_callback);
   	do{
        gpio_test_infor.gpio_test_table=gpio_test_table;
		gpio_test_infor.gpio_test_num=TABLE_ELEMENT_CNT(gpio_test_table);
		gpio_test_infor.time_s=t1;
	}while(0);*/

   // xTaskCreate(t1_callback,"t1_callback",1024,&gpio_test_infor,30,NULL);
	LOG_ERROR("gpio_input_output_demo\n");
    return;
}

static void gpio_irq_init(uint64_t gpio_num)
{
	 gpio_config_t gpio_config_prot;
	memset(&gpio_config_prot,0,sizeof(gpio_config_prot));
    gpio_config_prot.GPIO_Pin= gpio_num;
	
	gpio_config_prot.GPIO_Mode=GPIO_Mode_Input;
	gpio_config_prot.GPIO_IntrType=GPIO_PIN_INTR_NEGEDGE;
	gpio_config_prot.GPIO_Pullup=GPIO_PullUp_EN;
	gpio_config_prot.GPIO_Pulldown=GPIO_PullDown_DIS;
    gpio_config(&gpio_config_prot);
	//Register gpio handler
	gpio_intr_handler_register(GPIO_isr_callback,NULL);
	//Enable gpio intr
	xt_ints_on(1<<GPIO_INUM);
	//ETS_GPIO_INTR_ENABLE(); //Enable intr
}


void GPIO_isr_callback(void* arg)
{
	/*GPIO interrupt process*/
	uint32_t gpio_intr_status = 0;
	uint32_t gpio_intr_status_h = 0;
	uint32_t gpio_num =0;
	//disable the pending interrupt
	xt_ints_off(1<<GPIO_INUM);
	gpio_intr_status = READ_PERI_REG(GPIO_STATUS_REG);
	gpio_intr_status_h = READ_PERI_REG(GPIO_STATUS1_REG);
	LOG_ERROR("the interrupt come in,gpio_intr_staus = %d,gpio_intr_status_h=%d\n",gpio_intr_status,gpio_intr_status_h);
	//clear intr for gpio0-gpio31
	WRITE_PERI_REG((GPIO_STATUS_W1TC_REG),(READ_PERI_REG(GPIO_STATUS_W1TC_REG)|(gpio_intr_status)));
	//clear intr for gpio32-39
	WRITE_PERI_REG((GPIO_STATUS1_W1TC_REG),(READ_PERI_REG(GPIO_STATUS1_W1TC_REG)|(gpio_intr_status_h)));
	do{
		if(gpio_num < 32){
			if(gpio_intr_status&BIT(gpio_num))
			{
				LOG_ERROR("Intr GPIO= %d\n",gpio_num);
				break;
			}
		}
		else{
			if(gpio_intr_status_h&BIT(gpio_num - 32)){
				LOG_ERROR("Intr GPIO = %d\n",gpio_num);
				break;
			}
		}
		
	}while(++gpio_num<GPIO_PIN_COUNT);
	TaskEvt_t *evt = (TaskEvt_t *)osi_malloc(sizeof(TaskEvt_t));
	if(evt == NULL)
	{
		return;
	}
	
	switch(gpio_num)
	{
		case GPIO_NUM_35:
			evt->par = Button_Voice;
		break;
		case GPIO_NUM_34:
			evt->par = Button_OK;
			break;
		case GPIO_NUM_39:
			evt->par = Button_Down;
			break;
		case GPIO_NUM_36:
			evt->par = Button_Up;
			break;
		default:
			evt->par = Button_Back;
		break;
	}
	evt->sig = BUTTON_PRESS_EVT;
	if(xQueueSend(xBtaApp1Queue,&evt,10/portTICK_RATE_MS)!=pdTRUE){
		LOG_ERROR("btdm_post_failed\n");
	}
	//enable the interrupt
	xt_ints_on(1<<GPIO_INUM);
	LOG_ERROR("the interrupt come in arg = %s\n",arg);
}


uint8_t check_sum(uint8_t *check_array,uint8_t len)
{
	uint8_t i,sum = 0;
	for(i = 0;i < len; i++)
	{
		sum ^= check_array[i];
	}
	sum += 1;

	return sum;
}

#endif
