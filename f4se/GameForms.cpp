#include "f4se/GameForms.h"
#include "f4se/GameUtilities.h"

RelocAddr <_LookupFormByID> LookupFormByID(0x0030E8F0);

// 
RelocPtr <IFormFactory*> g_formFactoryList(0x030CF650);

// 
RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap(0x02EC4758-0x08);
