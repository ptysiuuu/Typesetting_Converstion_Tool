#pragma once
#include "ast.h"
#include <memory>
#include <string>

class GFMParser {
  public:
	std::unique_ptr<Document> parse(const std::string &markdown_text);
};