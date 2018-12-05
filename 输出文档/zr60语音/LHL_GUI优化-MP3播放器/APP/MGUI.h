#ifndef __MGUI_H_
#define __MGUI_H_

/******************Export*******************/
/**
  * @brief  MGUI��ʼ��
  * @param  None
  * @retval None
  */
void MGUI_Init(void);
/**
  * @brief  MGUI���̺���
  * @param  None
  * @retval None
  */
void MGUI_ProcessHandler(void);
/**
  * @brief  ���ñ���ָ��λ��
  * @param  NewSpeed���ٶȣ���Χ��0-21��
  * @retval None
  */
void MGUI_SetSpeed(float NewSpeed);
/**
  * @brief  ������̱�
  * @param  NewMile����ǰ���
  * @retval None
  */
void MGUI_SetMile(float NewMile);
/**
  * @brief  �����¶�
  * @param  NewTemprature����ǰ�¶ȣ���Χ 0��100
  * @retval None
  */
void MGUI_SetTemprature(float NewTemprature);
/**
  * @brief  ���õ���
  * @param  NewEnergy����ǰ��������Χ0��60
  * @retval None
  */
void MGUI_SetEnergy(float NewEnergy);
/**
  * @brief  ���ý���״̬
  * @param  NewState��1������ס��0����⿪
  * @retval None
  */
void MGUI_SetLockState(unsigned char NewState);
/**
  * @brief  ������Ҫ��ʾ��ID
  * @param  NewID����Ҫ��ʾ��ID
  * @retval None
  */
void MGUI_SetFingerID(int NewID);
/**
  * @brief  ����GUI��Ϣ
  * @param  N
  * @retval None
	* @verbatim ����Ӧ�������ʱ��Ҫ���ô˺�����֪ͨGUI������һ���ĸı�.һ������Ϣ���ࣺ
	* MSG_PageDeleteUser,֪ͨGUI�л���ָ��ɾ������
	* MSG_PageAddUser,֪ͨGUI�л���ָ����ӽ���
	* MSG_PageNormal,֪ͨGUI�л���������ʾ����
	* MSG_PressFinger,֪ͨGUI���ڵ���ָ�ư���
	* MSG_FingerHansRecord,֪ͨGUI��ǰע����ָ�Ѿ���ע��
	* MSG_Retry,֪ͨGUI��ʾ���³�����Ϣ
	* MSG_RegisterSuccess,֪ͨGUIָ��ע��ɹ�
	* MSG_RegisterFaild,֪ͨGUIָ��ע��ʧ��
	* MSG_DeleteSuccess,֪ͨGUIָ��ɾ���ɹ�
	* MSG_DeleteFaild,֪ͨGUIָ��ɾ��ʧ��
  */
void MGUI_SendMsg(short NewMsg);
#endif
