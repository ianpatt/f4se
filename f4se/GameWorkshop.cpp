#include "f4se/GameWorkshop.h"

RelocAddr <PowerUtils::_UpdateMovingWirelessItem> PowerUtils::UpdateMovingWirelessItem(0x0033A540); // Usually paired with LinkPower
// this was inlined everywhere
//RelocAddr <_GetObjectAtConnectPoint> GetObjectAtConnectPoint(0x001FF360); // Acquires objects that are touching attach points
RelocAddr <TerminalUtils::_EstablishTerminalLinks> TerminalUtils::EstablishTerminalLinks(0x0033B3D0);
RelocAddr <SplineUtils::_ConnectSpline> SplineUtils::ConnectSpline(0x00339870);
RelocAddr <SplineUtils::_UpdateSpline> SplineUtils::UpdateSpline(0x003393E0);

// 
RelocAddr <Workshop::_ScrapReference> Workshop::ScrapReference(0x0033F7C0);

RelocPtr<BSPointerHandle<TESObjectREFR>> Workshop::hCurrentWorkshop(0x02E74154);
RelocAddr<Workshop::_FindNearestValidWorkshop> Workshop::FindNearestValidWorkshop(0x00330FB0);
