//////////////////////////////////////////////////
// 
#pragma once

#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <Quantity_Color.hxx>

class kcShapeAttrib;
DEFINE_STANDARD_HANDLE(kcShapeAttrib,Standard_Transient);

// ��¼һ��STEP����ת���ɵ�OCC��shape��������Ϣ����ɫ��ͼ��ȡ�
// Ŀǰֻ֧����ɫ
class kcShapeAttrib : public Standard_Transient
{
public:
	kcShapeAttrib(void);
	~kcShapeAttrib(void);

	void					SetColor(double r,double g,double b);
	void					SetColor(const Quantity_Color &aCol);
	Quantity_Color		GetColor() const;
	bool					HasColor() const { return bHasColor_; }

public:
	DEFINE_STANDARD_RTTIEXT(kcShapeAttrib,Standard_Transient)

protected:
	bool					bHasColor_;//�Ƿ��Ѿ��趨����ɫ
	Quantity_Color		aColor_;//��ɫ���ԣ�RGBֵ
};

