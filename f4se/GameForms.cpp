#include "f4se/GameForms.h"
#include "f4se/GameUtilities.h"

RelocAddr <_LookupFormByID> LookupFormByID(0x0030EA30);

// 
RelocPtr <IFormFactory*> g_formFactoryList(0x030D05D0);

// 
RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap(0x02EC5758-0x08);
