#include "OcclusionQuery.h"

OcclusionQuery::OcclusionQuery()
{
    glGenQueries(1, &_query);
}

void OcclusionQuery::beginQuery()
{
    glBeginQuery(GL_SAMPLES_PASSED, _query);
}

void OcclusionQuery::endQuery()
{
    glEndQuery(GL_SAMPLES_PASSED);
}

bool OcclusionQuery::isAvailable()
{
    glGetQueryObjectiv(_query, GL_QUERY_RESULT_AVAILABLE, &_queryAvailabe);
    return _queryAvailabe != 0;
}

bool OcclusionQuery::isVisible()
{
    glGetQueryObjectiv(_query, GL_QUERY_RESULT_NO_WAIT, &_samplesCount);
    return _samplesCount > 0;
}