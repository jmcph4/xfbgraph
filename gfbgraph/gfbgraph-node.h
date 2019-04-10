/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * libgfbgraph - GObject library for Facebook Graph API
 * Copyright (C) 2013 Álvaro Peña <alvaropg@gmail.com>
 *
 * GFBGraph is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GFBGraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GFBGraph.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GFBGRAPH_NODE_H__
#define __GFBGRAPH_NODE_H__

#include <glib-object.h>
#include <gfbgraph/gfbgraph-authorizer.h>

G_BEGIN_DECLS

#define GFBGRAPH_TYPE_NODE (gfbgraph_node_get_type())

G_DECLARE_DERIVABLE_TYPE (GFBGraphNode, gfbgraph_node, GFBGRAPH, NODE, GObject)

#define GFBGRAPH_NODE_ERROR            gfbgraph_node_error_quark ()

struct _GFBGraphNodeClass
{
  GObjectClass parent_class;

#if 0
  gpointer _reserved[10];
#endif
};

typedef enum
{
  GFBGRAPH_NODE_ERROR_NO_CONNECTIONABLE = 1,
  GFBGRAPH_NODE_ERROR_NO_CONNECTABLE
} GFBGraphNodeError;

GQuark         gfbgraph_node_error_quark (void) G_GNUC_CONST;
GFBGraphNode*  gfbgraph_node_new         (void);

GFBGraphNode*  gfbgraph_node_new_from_id (GFBGraphAuthorizer  *authorizer,
                                          const gchar         *id,
                                          GType                node_type,
                                          GError             **error);

const gchar*   gfbgraph_node_get_id           (GFBGraphNode *node);
const gchar*   gfbgraph_node_get_link         (GFBGraphNode *node);
const gchar*   gfbgraph_node_get_created_time (GFBGraphNode *node);
const gchar*   gfbgraph_node_get_updated_time (GFBGraphNode *node);

void           gfbgraph_node_set_id           (GFBGraphNode *node,
                                               const gchar  *id);

GList*         gfbgraph_node_get_connection_nodes              (GFBGraphNode         *node,
                                                                GType                 node_type,
                                                                GFBGraphAuthorizer   *authorizer,
                                                                GError              **error);
void           gfbgraph_node_get_connection_nodes_async        (GFBGraphNode         *node, 
                                                                GType                 node_type, 
                                                                GFBGraphAuthorizer   *authorizer,
                                                                GCancellable         *cancellable,
                                                                GAsyncReadyCallback   callback,
                                                                gpointer              user_data);
GList*         gfbgraph_node_get_connection_nodes_async_finish (GFBGraphNode         *node,
                                                                GAsyncResult         *result,
                                                                GError              **error);

gboolean       gfbgraph_node_append_connection (GFBGraphNode        *node,
                                                GFBGraphNode        *connect_node,
                                                GFBGraphAuthorizer  *authorizer,
                                                GError             **error);

G_END_DECLS

#endif /* __GFBGRAPH_NODE_H__ */
