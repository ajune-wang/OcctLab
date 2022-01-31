//
#include "stdafx.h"
#include "Dlgs\ShapeStructDialog.h"
#include "QxSysLib.h"


// ��ʾshape����Ϣ��bCumOri:�Ӷ����Ƿ��Ǹ�����ķ���
void ShowShapeStructDialog(const TopoDS_Shape &aShape,bool bCumOri)
{
	if(aShape.IsNull())
		return;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CShapeStructDialog dlg(aShape);
	dlg.UseParentOri(bCumOri);
	dlg.DoModal();
}
