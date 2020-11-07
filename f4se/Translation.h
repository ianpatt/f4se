#pragma once

class BSScaleformTranslator;

namespace Translation
{
	void ImportTranslationFiles(BSScaleformTranslator * translator);
	void ParseTranslation(BSScaleformTranslator * translator, std::string & name);
}
