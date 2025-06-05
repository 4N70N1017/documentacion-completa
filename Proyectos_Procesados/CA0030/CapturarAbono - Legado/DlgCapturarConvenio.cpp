
/* --------------------------------------------------------------------
    Fecha    :26/10/04 6:21  ( dd-mm-aa )
    Elabor�  :Joel Armando Arana Garcia
    Solicit� :Lic. Jos� G. Mendoza
    Actividad:Captura de Convenio.
   -------------------------------------------------------------------- */
   // DlgCapturarConvenio.cpp : implementation file
   //

#include "stdafx.h"
#include "sysCapturarAbono.h"
#include "CapturarAbono.h"
#include "DlgCapturarAbono.h"
#include "DlgCapturarConvenio.h"
#include "gnwproto.hpp"
#include "CMaximo.hpp"
#include "ConsultarMesAnio.hpp"
#include "sysCapturarAbono.h"

#include <process.h>
#include "password.hpp"
#include "C_Menu.hpp"

// CDlgCapturarConvenio dialog
struct DatosConvenio
{
    int		 iTipo,
        lCliente,
        lFactura,
        iTipoConvenio,
        iPlazo,
        lImporteTabla,
        lEmpleado,
        iSubTipoConvenio;
    char	 cFechaMovimiento[25];
};

DatosConvenio ActuConve[100];

int iTotalConvenio;

IMPLEMENT_DYNAMIC(CDlgCapturarConvenio, CDialog2012)
CDlgCapturarConvenio::CDlgCapturarConvenio(CWnd* pParent /*=NULL*/)
    : CDialog2012(CDlgCapturarConvenio::IDD, pParent)
{

    iControles = iFoco = iDiaActual = iMesActual = iAnioActual = iFlagCapturo = iCaja = iMuestraMsg = iTotalVencido = iContadorConv = 0;
    imes = ianio = iTotalArreglo = 0;
    lEmpleado = lImporte = lSaldoCuenta = lFactura = lCliente = 0;
    ShiftTab = false;
    iPlazo = iTipoConvenio = iSubTipoConvenio = iTipo = 0;
}

CDlgCapturarConvenio::~CDlgCapturarConvenio()
{
	lCliente = 0;
	iCaja = 0;
	iControles = 0;
	iTotalVencido = 0;
	lEmpleado = 0;
	iMuestraMsg = 0;
	iAnioActual = 0;
	iMesActual = 0;
	iDiaActual = 0;
	iTipo = 0;
	lFactura = 0;
	iTipoConvenio = 0;
	iSubTipoConvenio = 0;
}

void CDlgCapturarConvenio::DoDataExchange(CDataExchange* pDX)
{
    CDialog2012::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EMPLEADO, m_empleado);
    DDX_Control(pDX, IDC_PLAZO, m_plazo);
    DDX_Control(pDX, IDC_IMPORTE, m_importe);
    DDX_Control(pDX, IDC_CANCELAR, m_cancelar);
    DDX_Control(pDX, IDC_ACEPTAR, m_aceptar);
}

BEGIN_MESSAGE_MAP(CDlgCapturarConvenio, CDialog2012)
    ON_BN_CLICKED(IDC_CANCELAR, OnBnClickedCancelar)
    ON_BN_CLICKED(IDC_ACEPTAR, OnBnClickedAceptar)
    ON_EN_SETFOCUS(IDC_EMPLEADO, OnEnSetfocusEmpleado)
    ON_EN_SETFOCUS(IDC_PLAZO, OnEnSetfocusPlazo)
    ON_EN_SETFOCUS(IDC_IMPORTE, OnEnSetfocusImporte)
END_MESSAGE_MAP()

// CDlgCapturarConvenio message handlers

BOOL CDlgCapturarConvenio::OnInitDialog()
{
    CDialog2012::OnInitDialog();
    CString sTexto;
    iContadorConv = 0;

    if (iSubTipoConvenio == 1)
    {
        SetWindowText("CAPTURAR CONVENIO DE PALABRA");
    }
    else
    {
        SetWindowText("CAPTURAR CONVENIO UNICO");
    }

    // TODO:  Add extra initialization here
    m_empleado.SetLimitText(8);
    sTexto.Format("%ld", lEmpleado);
    m_empleado.SetWindowText(sTexto);
    m_plazo.SetLimitText(1);
    m_importe.SetLimitText(9);
    iFoco = 0;
    iControles = 3;
    lEmpleado = 0L;
    lImporte = 0L;
    iPlazo = 0;
    iFlagCapturo = 0;

    crearMensaje(this, m_barra, m_mensaje);
    ponerMensaje(this, m_barra, m_mensaje, " [ ESC ] Cancelar  [F10] Aceptar ");

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgCapturarConvenio::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    char cPaso[255] = { 0 };

    ShiftTab = false;

    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
    {
        if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP)
        {
            iFoco--;
            if (iFoco < 0)	iFoco = 0;
            asignarFoco();
        }
        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_RIGHT)
        {
            if (!boton())
            {
                if (iFoco <= iControles) //numero de controles
                {
					SecureZeroMemory(cPaso, sizeof(cPaso));
                    if (validarControl(cPaso))
                    {
                        iFoco++;
                        if (iFoco > 2) iFoco = 2;
                        asignarFoco();
                    }
                    else
                    {
                        if (!ShiftTab)
                        {
                            AfxMessageBox(cPaso);
                        }
                    }
                    return TRUE;
                }
                else
                {
                    if (pMsg->wParam != VK_RETURN)
                    {
                        iFoco++;
                        iFoco++;
                        if (iFoco > 2) iFoco = 2;
                        asignarFoco();
                    }
                    else
                    {
                        asignarFoco();
                    }
                    return TRUE;
                }
            }
        }
        else
        {
            if (pMsg->wParam == VK_TAB && GetKeyState(VK_SHIFT) & 0x8000)
            {
                ShiftTab = true;
            }
            else
            {
                switch (pMsg->wParam)
                {
                case VK_F10:
                    OnBnClickedAceptar();
                    break;
                case VK_ESCAPE:
                    CDialog2012::OnCancel();
                    pMsg->wParam = NULL;
                    break;
                case VK_RIGHT:
                    if (boton())
                    {
                        iFoco++;
                        if (iFoco > 2)
                            iFoco = 0;
                        asignarFoco();
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    return CDialog2012::PreTranslateMessage(pMsg);
}

bool CDlgCapturarConvenio::validarClick(int nTmpFocus)
{
    int nElementoTmp, i;
    bool regresa = true;
    char cPaso[255] = { 0 };

    if (iFoco < nTmpFocus)
    {
        // validar control anterior
		SecureZeroMemory(cPaso, sizeof(cPaso));
        if (!validarControl(cPaso))
        {
            asignarFoco();
            if (iFoco != iControles)//numero de controles para los que se quieren se vea el mebsaje de error
            {
                if (!ShiftTab)
                {
                    AfxMessageBox(cPaso);
                }
            }
            regresa = false;
        }
        else
        {
            nElementoTmp = iFoco;
            iFoco = nTmpFocus;
            for (i = 0; i <= nTmpFocus; i++)
            {
                iFoco = i;
                //valida hacia abajo los controles
				SecureZeroMemory(cPaso, sizeof(cPaso));
                if (!validarControl(cPaso))
                {
                    break;
                }
            }
            asignarFoco();
        }
    }
    else
    {
        iFoco = nTmpFocus;
    }
    return regresa;
}

void CDlgCapturarConvenio::asignarFoco()
{

    switch (iFoco)
    {
    case 0:
        m_empleado.SetFocus();
        break;
    case 1:
        m_plazo.SetFocus();
        break;
    case 2:
        m_importe.SetFocus();
        break;
    case 3:
        m_cancelar.SetFocus();
        break;
    case 4:
        m_aceptar.SetFocus();
        break;
    default:
        break;
    }
}

bool CDlgCapturarConvenio::obtenerMesAnio()
{
    bool valorRegresa = true;
    char cSql[240] = { 0 };
    ConsultarMesAnio DatosFecha(&odbc, false);
    imes = ianio = 0;

	if(sprintf_s(cSql, "select num_anio,num_mes from fun_regresadatosretailestadisticas((select fecha from gndominio))") < 0){;}

    if (DatosFecha.Exec(cSql))
    {
        DatosFecha.activarCols();
        if (DatosFecha.Leer())
        {
            ianio = (int)DatosFecha.anio;//anio
            imes = (int)DatosFecha.mes;//anio
        }
    }

    return valorRegresa;
}

bool CDlgCapturarConvenio::validarControl(char * cCadena)
{
    bool valorRegresa = true;
    CString sTexto;

    switch (iFoco)
    {
    case 0:
        m_empleado.GetWindowText(sTexto);
        sTexto.Trim();
        lEmpleado = strtol(sTexto, NULL, 10);
        if (sTexto.IsEmpty())
        {
			if(sprintf_s(cCadena, 255, "%s", "El n�mero de Empleado no puede estar vacio...") < 0){;}
            lEmpleado = 0L;
            m_empleado.SetSel(0, 8);
            valorRegresa = false;
        }
        else
        {
            if (esEmpleado(lEmpleado) != 1)
            {
				if(sprintf_s(cCadena, 255, "%s", " NUMERO DE EMPLEADO INCORRECTO ") < 0){;}
                lEmpleado = 0L;
                m_empleado.SetSel(0, 8);
                valorRegresa = false;
            }
        }
        break;
    case 1:
    {
        int iPlazoMaximo = 0;

        CMaximo Tablax(&odbc, false);////Se agrego para realizar consulta para el numeroPlazos permitidos
        char cSql[100] = { 0 };
        obtenerMesAnio();//Obtener Mes Y A�o Actual para consultar Datos
		if(sprintf_s(cSql, "SELECT num_plazomaximo FROM ctl_variablesparaconvenios WHERE anio=%d AND mes =%d LIMIT 1", ianio, imes) < 0){;}
        if (Tablax.Exec(cSql))
        {
            Tablax.activarCols();
            if (Tablax.Leer())
            {
                iPlazoMaximo = (int)Tablax.Total;
            }
        }

        m_plazo.GetWindowText(sTexto);
        sTexto.Trim();
        iPlazo = (short)strtol(sTexto, NULL, 10);
        if (sTexto.IsEmpty())
        {
			if(sprintf_s(cCadena, 255, "%s", "El Plazo no puede estar vacio...") < 0){;}
            iPlazo = 0;
            m_plazo.SetSel(0, 1);
            valorRegresa = false;
        }
        else
        {
            if (iPlazo<1 || iPlazo > iPlazoMaximo)
            {
				if(sprintf_s(cCadena, 255, "%s", "EL PLAZO ES INCORRECTO") < 0){;}
                iPlazo = 0;
                m_plazo.SetSel(0, 1);
                valorRegresa = false;
            }
        }
    }
    break;
    case 2:
        m_importe.GetWindowText(sTexto);
        sTexto.Trim();
        lImporte = strtol(sTexto, NULL, 10);
        if (sTexto.IsEmpty() || lImporte == 0)
        {
			if(sprintf_s(cCadena, 255, "%s", "IMPORTE INCORRECTO") < 0){;}
            lImporte = 0;
            m_importe.SetSel(0, 9);
            valorRegresa = false;
        }
        else
        {
            if (lImporte > iTotalVencido)
            {
				if(sprintf_s(cCadena, 255, "%s", " LA CANTIDAD A CONVENIAR ES MAYOR AL SALDO VENCIDO ") < 0){;}
                lImporte = 0;
                m_importe.SetSel(0, 9);
                valorRegresa = false;
            }
        }
        break;
    default:
        break;
    }

    return valorRegresa;
}

bool CDlgCapturarConvenio::boton()
{
    CWnd *pWnd = GetFocus();
    bool valorRegresa = false;

    switch (pWnd->GetDlgCtrlID())
    {
    case IDC_CANCELAR:
        valorRegresa = true;
        break;
    case IDC_ACEPTAR:
        valorRegresa = true;
        break;
    }
    return valorRegresa;
}

bool CDlgCapturarConvenio::validarControles()
{
    bool bValorRegresa = false;
    int i, iFocoAnt;
    bool bandera = true;
    char cPaso[255] = { 0 };

    iFocoAnt = iFoco;

    for (i = 0; i < iControles; i++)
    {
        iFoco = i;
		SecureZeroMemory(cPaso, sizeof(cPaso));
        bandera = validarControl(cPaso);
        if (!bandera)
        {
            AfxMessageBox(cPaso);
            asignarFoco();
            break;
        }
    }

    if (bandera)
    {
        bValorRegresa = true;
    }

    iFoco = iFocoAnt;
    asignarFoco();
    return bValorRegresa;
}

void CDlgCapturarConvenio::OnBnClickedCancelar()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}

void CDlgCapturarConvenio::OnBnClickedAceptar()
{
    // TODO: Add your control notification handler code here
    if (validarControles())
    {
        grabarConvenio();
        OnCancel();
    }
}

void CDlgCapturarConvenio::OnEnSetfocusEmpleado()
{
    // TODO: Add your control notification handler code here
    validarClick(0);
}

void CDlgCapturarConvenio::OnEnSetfocusPlazo()
{
    // TODO: Add your control notification handler code here
    validarClick(1);
}

void CDlgCapturarConvenio::OnEnSetfocusImporte()
{
    // TODO: Add your control notification handler code here
    validarClick(2);
}

bool CDlgCapturarConvenio::grabarConvenio()
{
    bool bValorRegresa = false;
    if (grabarConvenioTablaCorrespondiente())
    {
        iFlagCapturo = 1;
        bValorRegresa = true;
    }
    return bValorRegresa;
}

void CDlgCapturarConvenio::asignarValor(int icontador)
{
    iTotalConvenio = icontador;
    iTotalArreglo = icontador;
}

bool CDlgCapturarConvenio::grabarConvenioTablaCorrespondiente()
{
    bool bContinuar = false;
    char cFechaMovimiento[25] = { 0 };
    long lImporteTabla = 0;
    int iHoraMovto, iMinutosMovto, iSegundosMovto;

    iHoraMovto = 0;
    iMinutosMovto = 0;
    iSegundosMovto = 0;

    if (iTotalConvenio == 0)
    {
        SecureZeroMemory(&ActuConve, sizeof(DatosConvenio) * 100);
    }
    iTotalArreglo = iTotalConvenio;
    lImporteTabla = lImporte;

	if(sprintf_s(cFechaMovimiento, "%02d-%02d-%04d %02d:%02d:%02d", iMesActual, iDiaActual, iAnioActual, iHoraMovto, iMinutosMovto, iSegundosMovto) < 0){;}
    cFechaMovimiento[19] = 0;

    ActuConve[iTotalArreglo].iTipo = iTipo;
    ActuConve[iTotalArreglo].lCliente = lCliente;
    ActuConve[iTotalArreglo].lFactura = lFactura;
    ActuConve[iTotalArreglo].iTipoConvenio = iTipoConvenio;
    ActuConve[iTotalArreglo].iPlazo = iPlazo;
    ActuConve[iTotalArreglo].lImporteTabla = lImporteTabla;
    ActuConve[iTotalArreglo].lEmpleado = lEmpleado;
    ActuConve[iTotalArreglo].iSubTipoConvenio = iSubTipoConvenio;
	if(sprintf_s(ActuConve[iTotalArreglo].cFechaMovimiento, cFechaMovimiento) < 0){;} //
    iTotalArreglo++;
    iTotalConvenio = iTotalArreglo;
    bContinuar = true;

    return  bContinuar;
}

bool CDlgCapturarConvenio::grabarConvenioTablaCorrespondienteValidado(int factura, int importe, short itipocta)
{
    bool bContinuar = false;
    char cSql[170] = { 0 };

    for (int x = 0; x < iTotalConvenio; x++)
    {
        if (factura == ActuConve[x].lFactura && itipocta == ActuConve[x].iTipo)
        {
			if(sprintf_s(cSql, "SELECT gnactualizarcaconvenio('%d','%ld','%ld','%d','%d','%ld','%ld','%s','%d')", ActuConve[x].iTipo, ActuConve[x].lCliente, ActuConve[x].lFactura, ActuConve[x].iTipoConvenio, ActuConve[x].iPlazo, importe, ActuConve[x].lEmpleado, ActuConve[x].cFechaMovimiento, ActuConve[x].iSubTipoConvenio) < 0){;}

            CMaximo FlagCoppel(&odbc_1);
            if (FlagCoppel.Exec(cSql))
            {
                FlagCoppel.activarCols();
                if (FlagCoppel.Leer())
                {
                    bContinuar = true;
                }
            }
            else
            {
                //Se obtiene el error
                FlagCoppel.Error();
                FlagCoppel.odbc->GetLastError(FlagCoppel.GetHstmt());
                grabarMensajeError("C", iCaja, (LPTSTR)(LPCTSTR)odbc.m_strServer, "CapturarAbono", "CDlgCapturarConvenio", "grabarConvenioTablaCorrespondiente", cSql, lEmpleado, "ERROR EN LA CONSULTA ( grabarconveniotablacorrespondiente dlgcapturarconvenio)", FlagCoppel.odbc, iMuestraMsg);
            }
        }
    }

    return  bContinuar;
}