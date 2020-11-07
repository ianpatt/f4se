#include "f4se/GameWorkshop.h"

RelocAddr <PowerUtils::_UpdateMovingWirelessItem> PowerUtils::UpdateMovingWirelessItem(0x00201B10); // Usually paired with LinkPower
RelocAddr <_GetObjectAtConnectPoint> GetObjectAtConnectPoint(0x001FF360); // Acquires objects that are touching attach points
RelocAddr <TerminalUtils::_EstablishTerminalLinks> TerminalUtils::EstablishTerminalLinks(0x00204610);
RelocAddr <SplineUtils::_ConnectSpline> SplineUtils::ConnectSpline(0x00200E50);
RelocAddr <SplineUtils::_UpdateSpline> SplineUtils::UpdateSpline(0x00200B50);

// B040A6E939E55D7B97BC6BE389FC17F455E63F06+83
RelocAddr <Workshop::_ScrapReference> Workshop::ScrapReference(0x002084C0);

RelocPtr<BSPointerHandle<TESObjectREFR>> Workshop::hCurrentWorkshop(0x058DFDBC);
RelocAddr<Workshop::_FindNearestValidWorkshop> Workshop::FindNearestValidWorkshop(0x001F5610);