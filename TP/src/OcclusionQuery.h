#ifndef OCCLUSIONQUERY_H
#define OCCLUSIONQUERY_H

#include <glad/glad.h>

class OcclusionQuery
{
public:
    OcclusionQuery();

    void beginQuery();
    void endQuery();

    bool isAvailable();
    bool isVisible();

    GLuint _query = 0;
    GLint _queryAvailabe = 1;
    GLint _samplesCount = 1;
};

#endif // OCCLUSIONQUERY_H