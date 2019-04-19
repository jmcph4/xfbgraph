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

#ifndef __GFBGRAPH_SIMPLE_AUTHORIZER_H__
#define __GFBGRAPH_SIMPLE_AUTHORIZER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GFBGRAPH_TYPE_SIMPLE_AUTHORIZER (gfbgraph_simple_authorizer_get_type())
G_DECLARE_DERIVABLE_TYPE (GFBGraphSimpleAuthorizer, gfbgraph_simple_authorizer, GFBGRAPH, SIMPLE_AUTHORIZER, GObject)

struct _GFBGraphSimpleAuthorizerClass
{
  GObjectClass  parent_class;

  gpointer      _reserved1;
  gpointer      _reserved2;
  gpointer      _reserved3;
  gpointer      _reserved4;
  gpointer      _reserved5;
  gpointer      _reserved6;
};

GFBGraphSimpleAuthorizer* gfbgraph_simple_authorizer_new      (const gchar *access_token);

G_END_DECLS

#endif /* __GFBGRAPH_SIMPLE_AUTHORIZER_H__ */
