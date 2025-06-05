#include "stdafx.h"
#include "DlgCapturarAbono.h"

void CDlgCapturarAbono::imprimirMensajeInvitacionBancoppel()
{
    CWnd pWnd;
    CClientDC dc(&pWnd);
    CGenerarImpresion termica("c:\\sys\\mem\\InvitacionBancoppel.prn", true);
    int iRenglon = 0;

    termica.iniciarImpresion("BIXOLON SAMSUNG SRP-350plus", false);
    termica.impresionHorizontal(IMPRESION_VERTICAL);
    termica.ponerTituloCoppelRFC(iRenglon, dc);
    iRenglon += 240;

    termica.alinearTexto(CENTRO);
    termica.tipoLetra(&cNewFont, "Arial", &pcOldFont, 11);
    termica.ponerTermica(iRenglon, 250, "Mensaje BanCoppel");

    termica.alinearTexto(IZQUIERDA);
    iRenglon += 30;
    termica.ponerLinea(iRenglon, 5, iRenglon, 500);

    //Cuerpo Mensaje
    termica.tipoLetra(&cNewFont, "Arial", &pcOldFont, 8);

    iRenglon += 25;

    termica.ponerTermica(iRenglon, 25, "Usted  es  un  muy  buen  Cliente  de  Coppel  y");
    iRenglon += 25;
    termica.ponerTermica(iRenglon, 25, "PROBABLEMENTE    PUEDA    obtener    su");
    iRenglon += 25;
    termica.ponerTermica(iRenglon, 25, "Tarjeta de Crédito BanCoppel VISA.");

    iRenglon += 40;

    termica.ponerTermica(iRenglon, 25, "Lo  invitamos  a  que  pase  a  BanCoppel  para");
    iRenglon += 25;
    termica.ponerTermica(iRenglon, 25, "que  realice  el  tramite.");
    iRenglon += 25;
    termica.ponerTermica(iRenglon, 25, "Es  muy  fácil.");
    iRenglon += 50;
    termica.ponerTermica(iRenglon, 0, ".");
    termica.terminarImpresion();
}

void CDlgCapturarAbono::ImprimirMensajeActualizarDatos()
{
    char cMensaje[160] = { 0 };
    int iRen = 0;
    CString sTexto = "";
    CWnd pWnd;
    CClientDC dc(&pWnd);
    CGenerarImpresion termica("C:\\sys\\mem\\MENSAJE.txt", true);

    sTexto.Format("%.15s", cNombre);
    sTexto.Trim();
	if(sprintf_s(cNombre, "%s", sTexto) < 0){;}

    sTexto.Format("%.15s", cApellidoPaterno);
    sTexto.Trim();
	if(sprintf_s(cApellidoPaterno, "%s", sTexto) < 0){;}

    sTexto.Format("%.15s", cApellidoMaterno);
    sTexto.Trim();
	if(sprintf_s(cApellidoMaterno, "%s", sTexto) < 0){;}

    termica.iniciarImpresion("BIXOLON SAMSUNG SRP-350plus", false);
    termica.tipoLetra(&cNewFont, "Arial", &pcOldFont, 8);
    termica.ponerTituloCoppelRFC(iRen, dc);
    termica.alinearTexto(IZQUIERDA);
    termica.tipoLetra(&cNewFont, "Arial Bolt", &pcOldFont, 8);
    iRen += 200;
    termica.ponerLinea(iRen, 0, iRen, 540);
    iRen += 50;

	if(sprintf_s(cMensaje, "Estimado Sr(a). %s %s %s", cNombre, cApellidoPaterno, cApellidoMaterno) < 0){;}
    termica.ponerParrafoTermica(cMensaje, iRen, 0, 500);
    iRen += 25;
    SecureZeroMemory(cMensaje, sizeof(cMensaje));
	if(sprintf_s(cMensaje, "Le informamos que es necesario que pase al área de clientes nuevos de su Tienda Coppel más cercana a actualizar sus datos para brindarle un mejor servicio") < 0){;}
    termica.ponerParrafoTermica(cMensaje, iRen, 0, 500);
    iRen += 50;
    termica.ponerParrafoTermica("Muchas Gracias.", iRen, 0, 500);

    iRen += 100;
    termica.ponerTermica(iRen + 10, 5, ".");
    termica.terminarImpresion();
}