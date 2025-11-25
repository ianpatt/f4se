#include "f4se/GameForms.h"
#include "f4se/GameUtilities.h"

RelocAddr <_LookupFormByID> LookupFormByID(0x0030EE20);

// 
RelocPtr <IFormFactory*> g_formFactoryList(0x030D57D0);

// 
RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap(0x02ECA758-0x08);
