#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libxml/SAX.h>
#include "Eo.h"
#include "common.h"
#include "xml_sax_base.h"

EAPI Eo_Op XML_SAX_BASE_BASE_ID = 0;

typedef struct
{
   Eo *handler;
   Eo *current_filter;
}
Private_Data;

#define MY_CLASS XML_SAX_BASE

// LIBXML Wrappers
static void
_libxml2_set_document_locator(void * ctx, xmlSAXLocatorPtr loc)
{
   LOG("_libxml2_set_document_locator");
   
   // Here we grab current filter and private data for such, if a handler exists we
   // use the handler callback if defined.
   Eo *filter = (Eo*)ctx;
   const Eo_Class *current_class = eo_class_get(filter);
   
   LOGF("Class is : %s %s", eo_class_name_get(current_class), __func__);
   
   Private_Data *data = eo_data_get(filter, current_class);
   
   Eo *handler = data->handler;
   
   if (handler != NULL)
   {
     eo_do(handler, set_document_locator(ctx, loc));
   } else {
     eo_do(filter, set_document_locator(ctx, loc));
   }
}

static void
_libxml2_document_start(void *user_data)
{
   LOG("_libxml2_document_start");
   Eo *filter = (Eo*)user_data;
   const Eo_Class *current_class = eo_class_get(filter);
   
   LOGF("Class is : %s %s", eo_class_name_get(current_class), __func__);
   
   Private_Data *data = eo_data_get(filter, current_class);
   
   Eo *handler = data->handler;
   
   if (handler != NULL)
   {
     eo_do(handler, document_start(user_data));
   } else {
     eo_do(filter, document_start(user_data));
   }
}

static void
_libxml2_document_end(void *user_data)
{
   LOG("_libxml2_document_end");
   Private_Data *data = (Private_Data*)user_data;
   Eo *self = data->current_filter;
   eo_do(self, document_end(data));
}

static void
_libxml2_start(
	       void *ctx,
	       const xmlChar *localname,
	       const xmlChar *prefix,
	       const xmlChar *URI,
	       int nb_namespaces,
	       const xmlChar **namespaces,
	       int nb_attributes,
	       int nb_defaulted,
	       const xmlChar **attributes )
{
   LOG("_libxml2_start");

   // Get the current filter from the libxml2 context
   Private_Data *data = (Private_Data*)ctx;
   Eo *self = data->current_filter;

   // Populate struct to pass around
   ElementData element_data;
   element_data.ctx = ctx;
   element_data.localname = localname;
   element_data.prefix = prefix;
   element_data.URI = URI;
   element_data.nb_namespaces = nb_namespaces;
   element_data.namespaces = namespaces;
   element_data.nb_attributes = nb_attributes;
   element_data.nb_defaulted = nb_defaulted;
   element_data.attributes = attributes;

   // Fire in the hole!
   eo_do(self, start(&element_data));
}

void
_libxml2_char(
	      void *user_data,
	      const xmlChar *string,
	      int string_len)
{
   LOG("_libxml2_char");
   Private_Data *data = (Private_Data*)user_data;
   Eo *self = data->current_filter;
   eo_do(self, char(data, string, string_len));
}

void
_libxml2_end(
	     void* ctx,
	     const xmlChar* localname,
	     const xmlChar* prefix,
	     const xmlChar* URI)
{

   LOG("_libxml2_end");

   // TODO move this to a statement which checks end tag name = filter name
   //
   ElementData element_data;
   element_data.ctx = ctx;
   element_data.localname = localname;
   element_data.prefix = prefix;
   element_data.URI = URI;

   // Get the current filter from the libxml2 context
   Private_Data *data = (Private_Data*)ctx;
   Eo *self = data->current_filter;
   eo_do(self, end(&element_data));
}

// EFL Functions
//
static void
_parse_string(Eo *obj, void *class_data, va_list *list)
{
   // Create a parser instance for this request.
   // TODO this currently is here as having one setup in the constructor
   // results in function references being lost in transit...
   xmlSAXHandler parser;
   memset(&parser, 0, sizeof(xmlSAXHandler));
   parser.initialized = XML_SAX2_MAGIC;

   // Grab XML string from args
   char *xmlString = va_arg(*list, char*);

   LOG("Setting document handlers.");
   parser.startDocument = _libxml2_document_start;
   parser.endDocument = _libxml2_document_end;
   LOG("Setting element handlers.");
   parser.startElementNs = _libxml2_start;
   parser.endElementNs = _libxml2_end;
   parser.characters = _libxml2_char;
   parser.setDocumentLocator = _libxml2_set_document_locator;

   if (xmlSAXUserParseMemory(&parser, obj, xmlString, strlen(xmlString)) < 0 )
{
   LOG("Issue parsing XML document");
};
}

static void
_set_document_locator(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{

   const Eo_Class *current_class = eo_class_get(obj);
   LOGF("Class is : %s %s", eo_class_name_get(current_class), __func__);
}

static void
_document_start(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{

   LOG("XML SAX BASE doc start called");
}

static void
_start(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{
   int i;

   ElementData *element = va_arg(*list, ElementData*);

   LOGF("%s %s\n", eo_class_name_get(MY_CLASS), __func__);

   LOGF("ELEMENT: %s", element->localname);

   if (element->nb_attributes > 0)
     {
	for (i = 0; element->attributes[i]; i++)
{
   LOGF("ATTRIBUTE %i %s", i, element->attributes[i]);
    }
  }
}

static void
_char(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{
   void *data = va_arg(*list, void*);
   const xmlChar *string = va_arg(*list, const xmlChar*);
   int length = va_arg(*list, int);

   // ref important bytes passed in if they arent whitespace.
   if (length > 0 && !isspace(*string))
     {
	LOGF("char data in tag: %i.%s", length, string);
    }
}

static void
_end(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{

   LOG("Element End");
   ElementData *element = va_arg(*list, ElementData*);
   LOGF("End element name: %s", element->localname);
}

static void
_document_end(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{
   LOG("Document end");
}

static void
_handler_constructor(Eo *obj, void *class_data, va_list *list)
{
   // Get handler from args
   Eo *handler = va_arg(*list, Eo*);
   
   // Assign the current handler.
   Private_Data *pd = class_data;
   pd->handler = handler;
   
   // Call base constructor.
   eo_do_super(obj, MY_CLASS, eo_constructor());
}

static void
_class_constructor(Eo_Class *klass)
{
   const Eo_Op_Func_Description func_desc[] =
{
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_HANDLER_CONSTRUCTOR), _handler_constructor),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_PARSE_STRING), _parse_string),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_SET_DOCUMENT_LOCATOR), _set_document_locator),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_DOCUMENT_START), _document_start),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_START), _start),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_CHAR), _char),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_END), _end),
   EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_DOCUMENT_END), _document_end),
   EO_OP_FUNC_SENTINEL
};

   eo_class_funcs_set(klass, func_desc);
}

static const Eo_Op_Description op_desc[] =
{
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_HANDLER_CONSTRUCTOR, "Custom constructor that allows setting of handler."),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_PARSE_STRING, "Starts processing an XML document."),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_SET_DOCUMENT_LOCATOR, "Recieves the document locator on startup"),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_DOCUMENT_START, "Called when parser starts processing document"),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_START, "Called when start tage hit during parsing"),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_CHAR, "Called when char data found between elements"),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_END, "Called when end tag is hit during parsing"),
   EO_OP_DESCRIPTION(XML_SAX_BASE_SUB_ID_DOCUMENT_END, "Called when end of document is reached during parsing"),
   EO_OP_DESCRIPTION_SENTINEL
};

static const Eo_Class_Description class_desc =
{
   EO_VERSION,
   "XML Sax Base",
   EO_CLASS_TYPE_REGULAR,
   EO_CLASS_DESCRIPTION_OPS(&XML_SAX_BASE_BASE_ID, op_desc, XML_SAX_BASE_SUB_ID_LAST),
   NULL,
   sizeof(Private_Data),
   _class_constructor,
   NULL
};

EO_DEFINE_CLASS(xml_sax_base_class_get, &class_desc, EO_BASE_CLASS, NULL);
