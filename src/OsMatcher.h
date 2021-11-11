//
// Created by friedrich on 27.10.21.
//

#ifndef OS_MATCHER_OSMATCHER_H
#define OS_MATCHER_OSMATCHER_H

#pragma once

#include <AppComponents/Common/Reader/CsvTrackReader.h>
#include "AppComponents/Common/Reader/GeoJsonMapReader.h"
#include "AppComponents/Common/Reader/JsonTrackReader.h"
#include "AppComponents/Common/Reader/OsmMapReader.h"

#include "AppComponents/Common/Matcher/GraphBuilder.h"
#include "AppComponents/Common/Matcher/Router.h"
#include "AppComponents/Common/Matcher/SamplingPointFinder.h"

#include "AppComponents/Common/Writer/CsvRouteWriter.h"
#include "AppComponents/Common/Writer/CsvSubRouteWriter.h"
#include "AppComponents/Common/Writer/GeoJsonMapWriter.h"
#include "AppComponents/Common/Writer/GeoJsonRouteWriter.h"
#include "AppComponents/Common/Writer/GeoJsonTrackWriter.h"
#include "AppComponents/Common/Writer/JsonRouteStatisticWriter.h"

namespace osmatcher = AppComponents::Common;

#endif  //OS_MATCHER_OSMATCHER_H
