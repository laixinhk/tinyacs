/* Tiny ACS test tool for developers.
 * Copyright (C) 2022-2024 Laixin LIU <liulaixin@hotmail.com>
 *
 * This file is part of Tiny ACS.
 *
 * Tiny ACS (Auto Configuration Server) is an open source
 * TR-069(CPE WAN Management Protocol) test tool for developers.
 * Tiny ACS is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * Tiny ACS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tiny ACS; see the file LICENSE.
 */

#include "cwmpH.h"


#if 1 //for soapcpp2 to generate example request xml files, keep these functions. 

SOAP_FMAC5 int SOAP_FMAC6 cwmp__GetParameterNames(struct soap* soap, struct _cwmp__GetParameterNames *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__GetParameterValues(struct soap* soap, struct _cwmp__GetParameterValues *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__GetParameterAttributes(struct soap* soap, struct _cwmp__GetParameterAttributes *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__SetParameterValues(struct soap* soap, struct _cwmp__SetParameterValues *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__SetParameterAttributes(struct soap* soap, struct _cwmp__SetParameterAttributes *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__Download(struct soap* soap, struct _cwmp__Download *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__Upload(struct soap* soap, struct _cwmp__Upload *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__AddObject(struct soap* soap, struct _cwmp__AddObject *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__DeleteObject(struct soap* soap, struct _cwmp__DeleteObject *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__GetRPCMethods(struct soap* soap, struct _cwmp__GetRPCMethods *send)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 cwmp__ChangeDUState(struct soap* soap, struct _cwmp__ChangeDUState *send)
{
    return SOAP_OK;
}

#endif
