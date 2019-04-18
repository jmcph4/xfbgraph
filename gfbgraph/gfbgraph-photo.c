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

/**
 * SECTION:gfbgraph-photo
 * @short_description: GFBGraph Photo node
 * @stability: Unstable
 * @include: gfbgraph/gfbgraph.h
 *
 * #GFBGraphPhoto represents a <ulink url="https://developers.facebook.com/docs/reference/api/photo/">photo in the Facebook Graph API</ulink>.
 **/

#include "gfbgraph-photo.h"
#include "gfbgraph-connectable.h"
#include "gfbgraph-album.h"

#include <json-glib/json-glib.h>
#include <libsoup/soup.h>
#include <libsoup/soup-request.h>
#include <libsoup/soup-request-http.h>
#include <libsoup/soup-requester.h>

typedef struct
{
  gchar              *name;
  gchar              *source;
  guint               width;
  guint               height;
  GList              *images;
  GFBGraphPhotoImage *hires_image;
} GFBGraphPhotoPrivate;

static void  gfbgraph_photo_connectable_iface_init    (GFBGraphConnectableInterface *iface);
static void  gfbgraph_photo_serializable_iface_init   (JsonSerializableIface *iface);

G_DEFINE_TYPE_WITH_CODE (GFBGraphPhoto, gfbgraph_photo, GFBGRAPH_TYPE_NODE,
                         G_ADD_PRIVATE (GFBGraphPhoto)
                         G_IMPLEMENT_INTERFACE (GFBGRAPH_TYPE_CONNECTABLE, gfbgraph_photo_connectable_iface_init)
                         G_IMPLEMENT_INTERFACE (JSON_TYPE_SERIALIZABLE, gfbgraph_photo_serializable_iface_init));

/* Properties */
enum {
  PROP_0,
  PROP_NAME,
  PROP_SOURCE,
  PROP_HEIGHT,
  PROP_WIDTH,
  PROP_IMAGES,
  N_PROPERTIES
};

static GParamSpec* properties [N_PROPERTIES];

#define GFBGRAPH_PHOTO_GET_PRIVATE(_obj) gfbgraph_photo_get_instance_private (GFBGRAPH_PHOTO (_obj))


/* --- GObject --- */
static void
gfbgraph_photo_finalize (GObject *object)
{
  GFBGraphPhotoPrivate *priv = GFBGRAPH_PHOTO_GET_PRIVATE (object);
  GList *images;
  GFBGraphPhotoImage *photo_image;

  images = priv->images;
  while (images) {
    photo_image = (GFBGraphPhotoImage *) images->data;

    g_free (photo_image->source);
    g_free (photo_image);

    images = g_list_next (images);
  }

  g_free (priv->name);
  g_free (priv->source);
  g_list_free (priv->images);

  G_OBJECT_CLASS (gfbgraph_photo_parent_class)->finalize (object);
}

static void
gfbgraph_photo_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GFBGraphPhotoPrivate *priv = GFBGRAPH_PHOTO_GET_PRIVATE (object);

  switch (prop_id) {
    case PROP_NAME:
      if (priv->name)
        g_free (priv->name);
      priv->name = g_strdup (g_value_get_string (value));
      break;

    case PROP_SOURCE:
      if (priv->source)
        g_free (priv->source);
      priv->source = g_strdup (g_value_get_string (value));
      break;

    case PROP_WIDTH:
      priv->width = g_value_get_uint (value);
      break;

    case PROP_HEIGHT:
      priv->height = g_value_get_uint (value);
      break;

    case PROP_IMAGES:
      /* TODO: Free GList memory with g_list_free_full */
      priv->images = g_value_get_pointer (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
gfbgraph_photo_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GFBGraphPhotoPrivate *priv = GFBGRAPH_PHOTO_GET_PRIVATE (object);

  switch (prop_id) {
    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;

    case PROP_SOURCE:
      g_value_set_string (value, priv->source);
      break;

    case PROP_WIDTH:
      g_value_set_uint (value, priv->width);
      break;

    case PROP_HEIGHT:
      g_value_set_uint (value, priv->height);
      break;

    case PROP_IMAGES:
      g_value_set_pointer (value, priv->images);

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
gfbgraph_photo_init (GFBGraphPhoto *object)
{
  GFBGraphPhotoPrivate *priv = GFBGRAPH_PHOTO_GET_PRIVATE (object);

  priv->images = NULL;
}

static void
gfbgraph_photo_class_init (GFBGraphPhotoClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = gfbgraph_photo_finalize;
  gobject_class->set_property = gfbgraph_photo_set_property;
  gobject_class->get_property = gfbgraph_photo_get_property;

  /**
   * GFBGraphPhoto:name:
   *
   * The name of the photo given by his owner.
   **/
  properties [PROP_NAME] =
    g_param_spec_string ("name", "The photo name",
                         "The name given by the user to the photo",
                         NULL,
                         G_PARAM_READWRITE);

  /**
   * GFBGraphPhoto:source:
   *
   * An URI for the photo, with a maximum width or height of 720px.
   **/
  properties [PROP_SOURCE] =
    g_param_spec_string ("source", "The URI for the photo",
                         "The URI for the photo, with a maximum width or height of 720px",
                         NULL,
                         G_PARAM_READWRITE);

  /**
   * GFBGraphPhoto:width:
   *
   * The default photo width, up to 720px.
   **/
  properties [PROP_WIDTH] =
    g_param_spec_uint ("width", "The photo width",
                       "The photo width",
                       0, G_MAXUINT, 0,
                       G_PARAM_READWRITE);

  /**
   * GFBGraphPhoto:height:
   *
   * The default photo height, up to 720px.
   **/
  properties [PROP_HEIGHT] =
    g_param_spec_uint ("height", "The photo height",
                       "The photo height",
                       0, G_MAXUINT, 0,
                       G_PARAM_WRITABLE);

  /**
   * GFBGraphPhoto:images:
   *
   * A list with the available representations of the photo, in differents sizes
   **/
  properties [PROP_IMAGES] =
    g_param_spec_pointer ("images", "Sizes of the photo",
                          "The different sizes available of the photo",
                          G_PARAM_READWRITE);

  g_object_class_install_properties (gobject_class, N_PROPERTIES, properties);
}

/* --- Private methods --- */
/* --- Implement GFBGraphConnectable interface --- */
GHashTable*
gfbgraph_photo_get_connection_post_params (GFBGraphConnectable *self,
                                           GType                node_type)
{
  GFBGraphPhotoPrivate *priv = GFBGRAPH_PHOTO_GET_PRIVATE (self);
  GHashTable *params;

  params = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (params, "message", priv->name);
  /* TODO: Incorpate the "source" param (multipart/form-data) */

  return params;
}

static void
gfbgraph_photo_connectable_iface_init (GFBGraphConnectableInterface *iface)
{
  GHashTable *connections;

  connections = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (connections, (gpointer) g_type_name (GFBGRAPH_TYPE_ALBUM), (gpointer) "photos");

  iface->connections = connections;
  iface->get_connection_post_params = gfbgraph_photo_get_connection_post_params;
  iface->parse_connected_data = gfbgraph_connectable_default_parse_connected_data;
}

/* --- Implement JsonSerialiable interface --- */
JsonNode *
gfbgraph_photo_serializable_serialize_property (JsonSerializable *serializable,
                                                const gchar      *property_name,
                                                const GValue     *value,
                                                GParamSpec       *pspec)
{
  JsonNode *node = NULL;

  g_print ("Serializing %s\n", property_name);

  if (g_strcmp0 ("images", property_name) == 0) {
  } else {
    node = json_serializable_default_serialize_property (serializable, property_name, value, pspec);
  }

  return node;
}

gboolean
gfbgraph_photo_serializable_deserialize_property (JsonSerializable *serializable,
                                                  const gchar      *property_name,
                                                  GValue           *value,
                                                  GParamSpec       *pspec,
                                                  JsonNode         *property_node)
{
  gboolean res;

  if (g_strcmp0 ("images", property_name) == 0) {
    if (JSON_NODE_HOLDS_ARRAY (property_node)) {
      guint i, num_images;
      JsonArray *jarray;
      GList *images = NULL;

      jarray = json_node_get_array (property_node);
      num_images = json_array_get_length (jarray);
      for (i = 0; i < num_images; i++) {
        JsonObject *image_object;
        GFBGraphPhotoImage *photo_image;

        image_object = json_array_get_object_element (jarray, i);
        photo_image = g_new0 (GFBGraphPhotoImage, 1);
        photo_image->width = json_object_get_int_member (image_object, "width");
        photo_image->height = json_object_get_int_member (image_object, "height");
        photo_image->source = g_strdup (json_object_get_string_member (image_object, "source"));

        images = g_list_append (images, photo_image);
      }

      g_value_set_pointer (value, (gpointer *) images);
      res = TRUE;
    } else {
      g_warning ("The 'images' node retrieved from the Facebook Graph API isn't an array, it's holding a %s\n", json_node_type_name (property_node));
      res = FALSE;
    }
  } else {
    res = json_serializable_default_deserialize_property (serializable, property_name, value, pspec, property_node);
  }

  return res;
}

GParamSpec*
gfbgraph_photo_serializable_find_property (JsonSerializable *serializable,
                                           const char       *name)
{
  return g_object_class_find_property (G_OBJECT_GET_CLASS (GFBGRAPH_PHOTO (serializable)), name);
}

GParamSpec**
gfbgraph_photo_serializable_list_properties (JsonSerializable *serializable,
                                             guint            *n_pspecs)
{
  return g_object_class_list_properties (G_OBJECT_GET_CLASS (GFBGRAPH_PHOTO (serializable)), n_pspecs);
}

void
gfbgraph_photo_serializable_set_property (JsonSerializable *serializable,
                                          GParamSpec       *pspec,
                                          const GValue     *value)
{
  g_object_set_property (G_OBJECT (serializable), g_param_spec_get_name (pspec), value);
}

void
gfbgraph_photo_serializable_get_property (JsonSerializable *serializable,
                                          GParamSpec       *pspec,
                                          GValue           *value)
{
  g_object_get_property (G_OBJECT (serializable), g_param_spec_get_name (pspec), value);
}

static void
gfbgraph_photo_serializable_iface_init (JsonSerializableIface *iface)
{
  iface->serialize_property   = gfbgraph_photo_serializable_serialize_property;
  iface->deserialize_property = gfbgraph_photo_serializable_deserialize_property;
  iface->find_property        = gfbgraph_photo_serializable_find_property;
  iface->list_properties      = gfbgraph_photo_serializable_list_properties;
  iface->set_property         = gfbgraph_photo_serializable_set_property;
  iface->get_property         = gfbgraph_photo_serializable_get_property;
}

/* --- Public APIs --- */

/**
 * gfbgraph_photo_new:
 *
 * Creates a new #GFBGraphPhoto.
 *
 * Returns: (transfer full): a new #GFBGraphPhoto; unref with g_object_unref()
 **/
GFBGraphPhoto*
gfbgraph_photo_new (void)
{
  return GFBGRAPH_PHOTO(g_object_new(GFBGRAPH_TYPE_PHOTO, NULL));
}

/**
 * gfbgraph_photo_new_from_id:
 * @authorizer: a #GFBGraphAuthorizer.
 * @id: a const #gchar with the photo ID.
 * @error: (allow-none): a #GError or %NULL.
 *
 * Retrieves an photo node from the Facebook Graph with the give ID.
 *
 * Returns: (transfer full): a new #GFBGraphPhoto; unref with g_object_unref()
 **/
GFBGraphPhoto*
gfbgraph_photo_new_from_id (GFBGraphAuthorizer  *authorizer,
                            const gchar         *id,
                            GError             **error)
{
  return GFBGRAPH_PHOTO (gfbgraph_node_new_from_id (authorizer,
                                                    id,
                                                    GFBGRAPH_TYPE_PHOTO,
                                                    error));
}


/**
 * gfbgraph_photo_download_default_size:
 * @photo: a #GFBGraphPhoto.
 * @authorizer: a #GFBGraphAuthorizer.
 * @error: (allow-none): a #GError or %NULL.
 *
 * Download the default sized photo pointed by @photo, with a maximum width or height of 720px.
 * The photo always is a JPEG.
 *
 * Returns: (transfer full): a #GInputStream with the photo content or %NULL in case of error.
 **/
GInputStream*
gfbgraph_photo_download_default_size (GFBGraphPhoto       *photo,
                                      GFBGraphAuthorizer  *authorizer,
                                      GError             **error)
{
  GInputStream *stream = NULL;
  SoupSession *session;
  SoupRequester *requester;
  SoupRequest *request;
  SoupMessage *message;
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);
  g_return_val_if_fail (GFBGRAPH_IS_AUTHORIZER (authorizer), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  session = soup_session_sync_new ();
  requester = soup_requester_new ();
  soup_session_add_feature (session, SOUP_SESSION_FEATURE (requester));

  request = soup_requester_request (requester, priv->source, error);
  if (request != NULL) {
    message = soup_request_http_get_message (SOUP_REQUEST_HTTP (request));

    stream = soup_request_send (request, NULL, error);
    if (stream != NULL) {
      g_object_weak_ref (G_OBJECT (stream), (GWeakNotify) g_object_unref, session);
    }

    g_clear_object (&message);
    g_clear_object (&request);
  }

  g_clear_object (&requester);

  return stream;
}

/**
 * gfbgraph_photo_get_name:
 * @photo: a #GFBGraphPhoto.
 *
 * Returns: (transfer none): the @photo name, which is the comment given by the user so it would by larger, or %NULL.
 **/
const gchar*
gfbgraph_photo_get_name (GFBGraphPhoto *photo)
{
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  return priv->name;
}

/**
 * gfbgraph_photo_get_source_uri:
 * @photo: a #GFBGraphPhoto.
 *
 * Returns: (transfer none): the image link with a maximun widht or height of 720px
 **/
const gchar*
gfbgraph_photo_get_default_source_uri (GFBGraphPhoto *photo)
{
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  return priv->source;
}

/**
 * gfbgraph_pohto_get_default_width:
 * @photo: a #GFBGraphPhoto.
 *
 * Returns: (transfer none): the default photo width, up to 720px.
 **/
guint
gfbgraph_photo_get_default_width (GFBGraphPhoto *photo)
{
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), 0);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  return priv->width;
}

/**
 * gfbgraph_pohto_get_default_height:
 * @photo: a #GFBGraphPhoto.
 *
 * Returns: (transfer none): the default photo height, up to 720px.
 **/
guint
gfbgraph_photo_get_default_height (GFBGraphPhoto *photo)
{
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), 0);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  return priv->height;
}

/**
 * gfbgraph_photo_get_images:
 * @photo: a #GFBGraphPhoto.
 *
 * Returns: (element-type GFBGraphPhotoImage) (transfer none): a #GList of #GFBGraphPhotoImage with the available photo sizes
 **/
GList*
gfbgraph_photo_get_images (GFBGraphPhoto *photo)
{
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  return priv->images;
}

/**
 * gfbgraph_photo_get_image_hires:
 * @photo: a #GFBGraphPhoto.
 *
 * Returns: (transfer none): a #GFBGraphPhotoImage with the higher resolution available of the photo
 **/
const GFBGraphPhotoImage*
gfbgraph_photo_get_image_hires (GFBGraphPhoto *photo)
{
  GFBGraphPhotoPrivate *priv;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  if (priv->hires_image == NULL) {
    GList *images_list;
    guint bigger_width = 0;
    GFBGraphPhotoImage *photo_image;

    images_list = priv->images;
    while (images_list) {
      photo_image = (GFBGraphPhotoImage *) images_list->data;
      if (photo_image->width > bigger_width) {
        priv->hires_image = photo_image;
        bigger_width = photo_image->width;
      }

      images_list = g_list_next (images_list);
    }
  }

  return priv->hires_image;
}

const GFBGraphPhotoImage*
gfbgraph_photo_get_image_near_width (GFBGraphPhoto *photo,
                                     guint          width)
{
  GFBGraphPhotoPrivate *priv;
  GList *images_list;
  GFBGraphPhotoImage *tmp_photo_image;
  GFBGraphPhotoImage *photo_image = NULL;
  gint tmp_w_dif, w_dif;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  images_list = priv->images;
  while (images_list) {
    tmp_photo_image = (GFBGraphPhotoImage *) images_list->data;
    tmp_w_dif = ABS (tmp_photo_image->width - width);

    if (photo_image == NULL || tmp_w_dif < w_dif) {
      w_dif = tmp_w_dif;
      photo_image = tmp_photo_image;
    } else {
    }

    images_list = g_list_next (images_list);
  }

  return photo_image;
}

const GFBGraphPhotoImage*
gfbgraph_photo_get_image_near_height (GFBGraphPhoto *photo,
                                      guint          height)
{
  GFBGraphPhotoPrivate *priv;
  GList *images_list;
  GFBGraphPhotoImage *tmp_photo_image;
  GFBGraphPhotoImage *photo_image = NULL;
  gint tmp_h_dif, h_dif;

  g_return_val_if_fail (GFBGRAPH_IS_PHOTO (photo), NULL);

  priv = GFBGRAPH_PHOTO_GET_PRIVATE (photo);

  images_list = priv->images;
  while (images_list) {
    tmp_photo_image = (GFBGraphPhotoImage *) images_list->data;
    tmp_h_dif = ABS(tmp_photo_image->height - height);

    if (photo_image == NULL || tmp_h_dif < h_dif) {
      h_dif = tmp_h_dif;
      photo_image = tmp_photo_image;
    }

    images_list = g_list_next (images_list);
  }

  return photo_image;
}
