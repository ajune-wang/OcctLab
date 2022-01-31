// ����һЩģ�������ڱ���Ͷ�ȡʱʹ�õ����ͺ�code
#ifndef _QX_STG_MODEL_DEF_H_
#define _QX_STG_MODEL_DEF_H_

//model�п����Ͷ���
#define KSTG_BLOCK_DOC_HEADER		0x01	 //doc�ļ����ļ�ͷ
#define KSTG_BLOCK_DOC_GLOBAL		0x10	 //doc�ļ���ȫ������
// model���ݿ�����
#define KSTG_BLOCK_MODEL			0x0100	//ģ������
#define KSTG_BLOCK_MODEL_END		0x0101	//ģ������

#define KSTG_BLOCK_LAYERS			0x0102	//ͼ��������ʼ
#define KSTG_BLOCK_LAYERS_END		0x0103	//ͼ��������ֹ
#define KSTG_BLOCK_LAYER			0x0104	//ͼ�����
#define KSTG_BLOCK_LAYER_END		0x0105	//ͼ�����
//
#define KSTG_BLOCK_ENTITY_TYPE		0x0106  //entity����
#define KSTG_BLOCK_ENTITY			0x0107  //entity
#define KSTG_BLOCK_ENTITY_ATTRIB   0x0108  //entity���Կ�

#define KSTG_BLOCK_BASEPLANES_BEGIN 	0x0110  //��׼������
#define KSTG_BLOCK_BASEPLANES_END		0x0111  //��׼������
#define KSTG_BLOCK_BASEPLANE			0x0112  //��׼������

//ֵ�Ե�code�Ĺ̶�ֵ,����Ǹ����ͣ�����ʹ�ø�ֵ
//������һЩ������Сֵ�����ڲ�ʹ��
#define KSTG_CV_COMMON_CODE_BASE  200
#define KSTG_CV_CODE_BLOCK_TAG		(KSTG_CV_COMMON_CODE_BASE+1)  //���Ψһ��ʶֵ
#define KSTG_CV_CODE_TYPE			(KSTG_CV_COMMON_CODE_BASE+2)   //����
#define KSTG_CV_CODE_NAME			(KSTG_CV_COMMON_CODE_BASE+3)   //����
#define KSTG_CV_CODE_HANDLE		(KSTG_CV_COMMON_CODE_BASE+4)   //���code
#define KSTG_CV_CODE_SHAPE			(KSTG_CV_COMMON_CODE_BASE+5)   //shape������
#define KSTG_CV_CODE_VISIBLE		(KSTG_CV_COMMON_CODE_BASE+6)   //�Ƿ�ɼ�
//
#define KSTG_CV_CODE_COLOR			(KSTG_CV_COMMON_CODE_BASE+7)   //��ɫ


#endif