#ifndef NDRNP_HEADER_H
#define NDRNP_HEADER_H

#include <cstddef>    // size_t
#include <cstdint>    // unitx_t

namespace ndrnp {
    typedef double         coordinate_type;
    typedef size_t         size_type;
    typedef int32_t        id_type;
    typedef int32_t        hop_type;
    typedef double         fit_type;

    // delay over each wireless link is 2 ms.
    const hop_type LINK_DELAY = 2;
    const double   TX         = 10.0;
    const hop_type HOP        = 6;

    // the number of sink nodes.
    size_type      sink_num = 1;
    // the number of sensor nodes.
    size_type      sensor_num = 25;
    // the number of candidate deployment locations (cdl),
    // the terms relay and cdl are used interchangeable 
    // through out this program.
    size_type      cdl_num = 400;
}

#endif
