#include "f4se/GameForms.h"
#include "f4se/GameUtilities.h"

RelocAddr <_LookupFormByID> LookupFormByID(0x00311B80);

// 
RelocPtr <IFormFactory*> g_formFactoryList(0x030EB9D0);

// 
RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap(0x02EE0758-0x08);
