#pragma once

#include "parser_handler.h"


class Parser
{
public:
    static Model newModel(const char* path);

    static Model* newModelPtr(const char* path);

    static Parser& instance();

    void parse(Model& m, const char* path);

private:
    Parser();
    Parser(const Parser&) = delete;
    void operator=(const Parser&) = delete;

    std::unique_ptr<ParserHandler> handler;
};
