/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * libgfbgraph - GObject library for Facebook Graph API
 * Copyright (C) 2013-2014 Álvaro Peña <alvaropg@gmail.com>
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

#ifndef __GFBGRAPH_USER_H__
#define __GFBGRAPH_USER_H__

#include <gio/gio.h>
#include <glib-object.h>

#include <gfbgraph/gfbgraph-node.h>

G_BEGIN_DECLS

#define GFBGRAPH_TYPE_USER (gfbgraph_user_get_type())
G_DECLARE_DERIVABLE_TYPE (GFBGraphUser, gfbgraph_user, GFBGRAPH, USER, GFBGraphNode)

struct _GFBGraphUserClass
{
  GFBGraphNodeClass parent_class;

  gpointer  _reserved1;
  gpointer  _reserved2;
  gpointer  _reserved3;
  gpointer  _reserved4;
  gpointer  _reserved5;
  gpointer  _reserved6;
};

GFBGraphUser* gfbgraph_user_new       (void);
GFBGraphUser* gfbgraph_user_new_from_id (GFBGraphAuthorizer  *authorizer,
                                         const gchar         *id,
                                         GError             **error);

GFBGraphUser* gfbgraph_user_get_me              (GFBGraphAuthorizer  *authorizer,
                                                 GError             **error);
void          gfbgraph_user_get_me_async        (GFBGraphAuthorizer  *authorizer,
                                                 GCancellable        *cancellable,
                                                 GAsyncReadyCallback  callback,
                                                 gpointer             user_data);
GFBGraphUser* gfbgraph_user_get_me_async_finish (GFBGraphAuthorizer  *authorizer,
                                                 GAsyncResult        *result,
                                                 GError             **error);

GList*        gfbgraph_user_get_albums              (GFBGraphUser        *user,
                                                     GFBGraphAuthorizer  *authorizer,
                                                     GError             **error);
void          gfbgraph_user_get_albums_async        (GFBGraphUser        *user,
                                                     GFBGraphAuthorizer  *authorizer,
                                                     GCancellable        *cancellable,
                                                     GAsyncReadyCallback  callback,
                                                     gpointer             user_data);
GList*        gfbgraph_user_get_albums_async_finish (GFBGraphUser  *user,
                                                     GAsyncResult  *result,
                                                     GError       **error);

const gchar*  gfbgraph_user_get_name  (GFBGraphUser *user);
const gchar*  gfbgraph_user_get_email (GFBGraphUser *user);

G_END_DECLS

#endif /* __GFBGRAPH_USER_H__ */
