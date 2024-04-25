#include "f4se/GameForms.h"
#include "f4se/GameUtilities.h"

RelocAddr <_LookupFormByID> LookupFormByID(0x002BD410);

// 
RelocPtr <IFormFactory*> g_formFactoryList(0x02E68490);

// 
RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap(0x02C6E478-0x08);
