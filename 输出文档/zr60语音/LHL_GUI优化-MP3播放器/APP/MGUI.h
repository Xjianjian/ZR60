#ifndef __MGUI_H_
#define __MGUI_H_

/******************Export*******************/
/**
  * @brief  MGUI初始化
  * @param  None
  * @retval None
  */
void MGUI_Init(void);
/**
  * @brief  MGUI进程函数
  * @param  None
  * @retval None
  */
void MGUI_ProcessHandler(void);
/**
  * @brief  设置表盘指针位置
  * @param  NewSpeed：速度，范围是0-21；
  * @retval None
  */
void MGUI_SetSpeed(float NewSpeed);
/**
  * @brief  设置里程表
  * @param  NewMile：当前里程
  * @retval None
  */
void MGUI_SetMile(float NewMile);
/**
  * @brief  设置温度
  * @param  NewTemprature：当前温度，范围 0到100
  * @retval None
  */
void MGUI_SetTemprature(float NewTemprature);
/**
  * @brief  设置电量
  * @param  NewEnergy：当前电量。范围0到60
  * @retval None
  */
void MGUI_SetEnergy(float NewEnergy);
/**
  * @brief  设置解锁状态
  * @param  NewState：1代表锁住，0代表解开
  * @retval None
  */
void MGUI_SetLockState(unsigned char NewState);
/**
  * @brief  设置需要显示的ID
  * @param  NewID：需要显示的ID
  * @retval None
  */
void MGUI_SetFingerID(int NewID);
/**
  * @brief  发送GUI消息
  * @param  N
  * @retval None
	* @verbatim 在相应任务完成时需要调用此函数以通知GUI发生像一个的改变.一下是消息种类：
	* MSG_PageDeleteUser,通知GUI切换到指纹删除界面
	* MSG_PageAddUser,通知GUI切换到指纹添加界面
	* MSG_PageNormal,通知GUI切换到常规显示界面
	* MSG_PressFinger,通知GUI正在等在指纹按下
	* MSG_FingerHansRecord,通知GUI当前注册手指已经被注册
	* MSG_Retry,通知GUI显示重新尝试信息
	* MSG_RegisterSuccess,通知GUI指纹注册成功
	* MSG_RegisterFaild,通知GUI指纹注册失败
	* MSG_DeleteSuccess,通知GUI指纹删除成功
	* MSG_DeleteFaild,通知GUI指纹删除失败
  */
void MGUI_SendMsg(short NewMsg);
#endif
