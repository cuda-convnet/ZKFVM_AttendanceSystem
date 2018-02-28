#ifndef libzkfv_errdef_h
#define libzkfv_errdef_h

/**
*	@file		libzkfverrdef.h
*	@brief		ָ����SDK-�����붨��
*	@author		scar chen
*	@date		2016-01-26
*	@version	1.0
*	@par	��Ȩ��
*				ZKTeco
*	@par	��ʷ�汾			
*
*	@note
*
*/

#define ZKFV_ERR_ALREADY_INIT	1	/**<	�Ѿ���ʼ�� */
#define ZKFV_ERR_OK			0	/**<	�����ɹ� */
#define ZKFV_ERR_INITLIB	-1	/**<	��ʼ���㷨��ʧ�� */
#define ZKFV_ERR_INIT		-2	/**<	��ʼ���ɼ���ʧ�� */
#define ZKFV_ERR_NO_DEVICE	-3	/**<	���豸���� */
#define ZKFV_ERR_NOT_SUPPORT	-4	/**<	�ӿ��ݲ�֧�� */
#define ZKFV_ERR_INVALID_PARAM	-5	/**<	��Ч���� */
#define ZKFV_ERR_OPEN			-6	/**<	���豸ʧ�� */
#define ZKFV_ERR_INVALID_HANDLE	-7	/**<	��Ч��� */
#define ZKFV_ERR_CAPTURE		-8	/**<	ȡ��ʧ�� */
#define ZKFV_ERR_EXTRACT_FP		-9	/**<	��ȡָ��ģ��ʧ�� */
#define ZKFV_ERR_ABSORT			-10	/**<	�ж� */
#define ZKFV_ERR_MEMORY_NOT_ENOUGH			-11	/**<	�ڴ治�� */
#define ZKFV_ERR_BUSY			-12	/**<	��ǰ���ڲɼ� */
#define ZKFV_ERR_ADD_FINGER		-13	/**<	���ָ��ģ��ʧ�� */
#define ZKFV_ERR_DEL_FINGER		-14	/**<	ɾ��ָ��ʧ�� */
#define ZKFV_ERR_ADD_VEIN		-15	/**<	��Ӿ���ģ��ʧ�� */
#define ZKFV_ERR_DEL_VEIN		-16	/**<	ɾ��������ʧ�� */
#define ZKFV_ERR_FAIL			-17	/**<	����ʧ�� */
#define ZKFV_ERR_CANCEL			-18	/**<	ȡ���ɼ� */
#define ZKFV_ERR_EXTRACT_FV		-19	/**<	��ȡָ����ģ��ʧ�� */
#define ZKFV_ERR_VERIFY_FP		-20 /**<	�ȶ�ָ��ʧ�� */
#define ZKFV_ERR_VERIFY_FV		-21 /**<	�ȶ�ָ����ʧ�� */
#define ZKFV_ERR_MERGE			-22 /**<	�ϲ��Ǽ�ָ��ģ��ʧ��	*/
#define ZKFV_ERR_FP_EXIST		-23	/**<	�ظ�ָ��ID	*/

#endif