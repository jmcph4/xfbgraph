/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*-  */
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

#ifndef __GFBGRAPH_PHOTO_H__
#define __GFBGRAPH_PHOTO_H__

#include <gio/gio.h>
#include <gfbgraph/gfbgraph-node.h>

G_BEGIN_DECLS

#define GFBGRAPH_TYPE_PHOTO (gfbgraph_photo_get_type())
G_DECLARE_DERIVABLE_TYPE (GFBGraphPhoto, gfbgraph_photo, GFBGRAPH, PHOTO, GFBGraphNode)

struct _GFBGraphPhotoClass {
        GFBGraphNodeClass parent_class;
};

typedef struct _GFBGraphPhotoImage GFBGraphPhotoImage;

/**
 * GFBGraphPhotoImage:
 *
 * An struct with the information of a image.
 */
struct _GFBGraphPhotoImage {
        guint  width;
        guint  height;
        gchar *source;
};

GFBGraphPhoto* gfbgraph_photo_new      (void);
GFBGraphPhoto* gfbgraph_photo_new_from_id (GFBGraphAuthorizer *authorizer, const gchar *id, GError **error);
GInputStream*  gfbgraph_photo_download_default_size (GFBGraphPhoto *photo, GFBGraphAuthorizer *authorizer, GError **error);

const gchar*        gfbgraph_photo_get_name               (GFBGraphPhoto *photo);
const gchar*        gfbgraph_photo_get_default_source_uri (GFBGraphPhoto *photo);
guint               gfbgraph_photo_get_default_width      (GFBGraphPhoto *photo);
guint               gfbgraph_photo_get_default_height     (GFBGraphPhoto *photo);
GList*              gfbgraph_photo_get_images             (GFBGraphPhoto *photo);
const GFBGraphPhotoImage* gfbgraph_photo_get_image_hires        (GFBGraphPhoto *photo);
const GFBGraphPhotoImage* gfbgraph_photo_get_image_near_width   (GFBGraphPhoto *photo, guint width);
const GFBGraphPhotoImage* gfbgraph_photo_get_image_near_height  (GFBGraphPhoto *photo, guint height);

G_END_DECLS

#endif /* __GFBGRAPH_PHOTO_H__ */
