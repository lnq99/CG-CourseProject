#include "parser.h"

Parser::Parser()
//    : handler(std::make_unique<Face3Handler>())
{
    // сортировать по частоте встречаемости
    auto chain = (new Face3Handler)
            ->add(new VertexHandler);

    handler = std::unique_ptr<ParserHandler>(chain);
}


Parser& Parser::instance()
{
    static Parser parser;
    // if use Qt
    // setlocale(LC_NUMERIC, "C");
    return parser;
}

Model Parser::newModel(const char* path)
{
    Model m;
    instance().parse(m, path);
    return m;
}

Model* Parser::newModelPtr(const char* path)
{
    Model *m = new Model();
    instance().parse(*m, path);
    return m;
}


void Parser::parse(Model& m, const char* path)
{
    FILE *f = fopen(path, "r");
    if (f)
    {
        char *line = NULL;
        size_t n = 0;

        while (getline(&line, &n, f) > 0)
        {
            handler->handle(m, line);
        }

        fclose(f);
    }
}
