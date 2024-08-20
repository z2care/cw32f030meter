/**
 * @file cw32f030_ram.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/*******************************************************************************
*
* �������ɺ�������Ϣ
* �人��Դ�뵼�����޹�˾������ʹ�����б�̴���ʾ���ķ�ר���İ�Ȩ���ɣ��������ɴ�
* ���ɸ��������ض���Ҫ�����Ƶ����ƹ��ܡ����ݲ��ܱ��ų����κη�����֤���人��Դ��
* �������޹�˾������򿪷��̺͹�Ӧ�̶Գ������֧�֣�����У����ṩ�κ���ʾ��
* ���ı�֤�������������������ڰ������й������ԡ�������ĳ���ض���;�ͷ���Ȩ�ı�֤
* ��������
* ���ۺ������Σ��人��Դ�뵼�����޹�˾������򿪷��̻�Ӧ�̾��������и����
* ��ʹ����֪�䷢���Ŀ�����ʱ��Ҳ����ˣ����ݵĶ�ʧ���𻵣�ֱ�ӵġ��ر�ġ�������
* ���ӵ��𺦣����κκ���Ծ����𺦣�������ҵ�����롢������Ԥ�ڿɽ�ʡ����
* ��ʧ��
* ĳЩ˾��Ͻ����������ֱ�ӵġ������Ļ����Ե������κε��ų������ƣ����ĳЩ��
* ȫ�������ų������ƿ��ܲ�������������
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CW32F03x_RAM_H__
#define __CW32F03x_RAM_H__

#ifdef __cplusplus
 extern "C" {
#endif

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "base_types.h"
#include "cw32f030.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
//============================================================

/** @defgroup �жϱ�־λ 
  * @{
  */
#define RAM_IT_PARITY                   (bv1)
#define IS_RAM_IT(IT)                   (((IT) & 0xFFFFFFFD == 0x0UL) && ((IT) != 0x0UL))
#define IS_RAM_GET_IT(IT)               ((IT) == RAM_IT_PARITY)

//============================================================ 

//RAM PARITY�ж�ʹ������
void RAM_ITConfig(uint32_t RAM_IT, FunctionalState NewState);
//��ȡRAM PARITY�жϱ�־λ
ITStatus RAM_GetITStatus(uint32_t RAM_IT);
//���RAM PARITY�ϱ�־λ
void RAM_ClearITPendingBit(uint32_t RAM_IT);
//��ȡ������żУ�����RAM��ַ//
uint32_t RAM_GetErrorAddress(void);

//============================================================

#ifdef __cplusplus
}
#endif

#endif