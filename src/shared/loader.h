#pragma once

#include "../shared/new_vis_graph.h"

typedef boost::function<void(vis_graph&, vis_coord&, vis_coord&)> graph_loader;
