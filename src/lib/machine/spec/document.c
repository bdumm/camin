#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <expat.h>
#include "Eo.h"
#include "common.h"
#include "amin.h"
#include "xml_sax_base.h"
#include "elt.h"
#include "document.h"

int DEPTH;

EAPI Eo_Op AMIN_MACHINE_SPEC_DOCUMENT_BASE_ID = 0;

static const char FILTER_TAG[] = "filter";
static const char BUNDLE_TAG[] = "bundle";
static const char GENERATOR_TAG[] = "generator";
static const char HANDLER_TAG[] = "handler";
static const char LOG_TAG[] = "log";

static const char ELEMENT_TAG[] = "element";
static const char NAMESPACE_TAG[] = "namespace";
static const char NAME_TAG[] = "name";
static const char MACHINE_TAG[] = "machine_name";
static const char POSITION_TAG[] = "position";

typedef struct
{
   char *localname;
   void **attributes;
   Eina_List *bundle;
   Eina_List *filters;
   char *element;
   Eo *mparent;   
   char *log;   
   char *filter_param;   
   char *download;  
   char *module;
   char *version;
   char *generator;
   char *handler_name;
   char *handler_out;
   const char *element_name;
   const char *name;
   const char *machine_name;
   const char *position;
   const char *name_space;
} Private_Data;

#define MY_CLASS AMIN_MACHINE_SPEC_DOCUMENT

_start_document ( Eo *obj EINA_UNUSED, void *class_data, va_list *list )
{
  LOG("IN DOCUMENT START DOCUMENT FILTER");
  
  ElementData *element = va_arg ( *list, ElementData* );
  
  const Eo_Class *current_class = eo_class_get ( obj );
  LOGF ( "Class is : %s %s", eo_class_name_get ( current_class ), __func__ );
}

static void 
_start(Eo *obj EINA_UNUSED, void *class_data, va_list *list) {
  int i;
  
  Private_Data *pd = class_data;
  ElementData *element = va_arg ( *list, ElementData* );
  
  pd->attributes = element->attributes;
  pd->localname = element->localname;
  
  // Get Module Name 
  if ( strncmp ( element->localname,FILTER_TAG,sizeof ( FILTER_TAG ) ) == 0 || strncmp ( element->localname,BUNDLE_TAG,sizeof ( BUNDLE_TAG ) ) == 0)
  {
    if ( element->nb_attributes > 0 )
                {
                  int i = 0;
                  int attribute_position = 0;
                  char *parse_value;

                  // Get values from attributes
                  while ( i < element->nb_attributes )
                    {
                      if ( element->attributes[attribute_position] != NULL )
                        {
                          if ( strncmp ( element->attributes[attribute_position],"name",sizeof ( element->attributes[attribute_position] ) ) == 0 )
                            {
                              int attribute_length = ( strlen ( element->attributes[attribute_position + 3] ) - strlen ( element->attributes[attribute_position + 4] ) );
                              pd->module = strndup ( element->attributes[attribute_position + 3], attribute_length );
                            }
                        }
                      attribute_position = attribute_position + 5;
                      i++;
                    }
		}
  }
  
  if ( strncmp ( element->localname,GENERATOR_TAG,sizeof ( GENERATOR_TAG ) ) == 0 )
  {
    if ( element->nb_attributes > 0 )
                {
                  int i = 0;
                  int attribute_position = 0;
                  char *parse_value;

                  // Get values from attributes
                  while ( i < element->nb_attributes )
                    {
                      if ( element->attributes[attribute_position] != NULL )
                        {
                          if ( strncmp ( element->attributes[attribute_position],"generator",sizeof ( element->attributes[attribute_position] ) ) == 0 )
                            {
                              int attribute_length = ( strlen ( element->attributes[attribute_position + 3] ) - strlen ( element->attributes[attribute_position + 4] ) );
                              pd->generator = strndup ( element->attributes[attribute_position + 3], attribute_length );
                            }
                        }
                      attribute_position = attribute_position + 5;
                      i++;
                    }
		}
  }
  
  if ( strncmp ( element->localname,HANDLER_TAG,sizeof ( HANDLER_TAG ) ) == 0 )
  {
    if ( element->nb_attributes > 0 )
                {
                  int i = 0;
                  int attribute_position = 0;
                  char *parse_value;

                  // Get values from attributes
                  while ( i < element->nb_attributes )
                    {
                      if ( element->attributes[attribute_position] != NULL )
                        {
                          if ( strncmp ( element->attributes[attribute_position],"handler",sizeof ( element->attributes[attribute_position] ) ) == 0 )
                            {
                              int attribute_length = ( strlen ( element->attributes[attribute_position + 3] ) - strlen ( element->attributes[attribute_position + 4] ) );
                              pd->handler_name = strndup ( element->attributes[attribute_position + 3], attribute_length );
                            }
                            
                            if ( strncmp ( element->attributes[attribute_position],"output",sizeof ( element->attributes[attribute_position] ) ) == 0 )
                            {
                              int attribute_length = ( strlen ( element->attributes[attribute_position + 3] ) - strlen ( element->attributes[attribute_position + 4] ) );
                              pd->handler_out = strndup ( element->attributes[attribute_position + 3], attribute_length );
                            }
                        }
                      attribute_position = attribute_position + 5;
                      i++;
                    }
		}
  }
  
  if ( strncmp ( element->localname,LOG_TAG,sizeof ( LOG_TAG ) ) == 0 )
  {
    if ( element->nb_attributes > 0 )
                {
                  int i = 0;
                  int attribute_position = 0;
                  char *parse_value;

                  // Get values from attributes
                  while ( i < element->nb_attributes )
                    {
                      if ( element->attributes[attribute_position] != NULL )
                        {
                          if ( strncmp ( element->attributes[attribute_position],"log",sizeof ( element->attributes[attribute_position] ) ) == 0 )
                            {
                              int attribute_length = ( strlen ( element->attributes[attribute_position + 3] ) - strlen ( element->attributes[attribute_position + 4] ) );
                              pd->generator = strndup ( element->attributes[attribute_position + 3], attribute_length );
                            }
                        }
                      attribute_position = attribute_position + 5;
                      i++;
                    }
		}
  }
  
  LOGF("module value %s\n", pd->module);
  
  LOGF("%s %s\n", eo_class_name_get(MY_CLASS), __func__);
  
  LOG("IN DOCUMENT FILTER");
} 

static void
_char(Eo *obj EINA_UNUSED, void *class_data, va_list *list)
{
   void *data = va_arg ( *list, void*);
   const xmlChar *string = va_arg(*list, const xmlChar*);
   int length = va_arg(*list, int);
   Private_Data *pd = class_data;

   // ref important bytes passed in if they arent whitespace.
   if (length > 0 && !isspace(*string))
     {
	if ( strncmp ( pd->localname,ELEMENT_TAG,sizeof ( ELEMENT_TAG ) ) == 0 )
	{
	  pd->element_name = string;
	}
	if ( strncmp ( pd->localname,ELEMENT_TAG,sizeof ( NAMESPACE_TAG ) ) == 0 )
	{
	  pd->name_space = string;
	}
	if ( strncmp ( pd->localname,ELEMENT_TAG,sizeof ( NAME_TAG ) ) == 0 )
	{
	  pd->name = string;
	}
	if ( strncmp ( pd->localname,ELEMENT_TAG,sizeof ( MACHINE_TAG ) ) == 0 )
	{
	  pd->machine_name = string;
	}
	if ( strncmp ( pd->localname,ELEMENT_TAG,sizeof ( POSITION_TAG ) ) == 0 )
	{
	  pd->position = string;
	}
    }
}

static void
_class_constructor(Eo_Class *klass)
{
  const Eo_Op_Func_Description func_desc[] = {
    EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_DOCUMENT_START), _start_document),
    EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_START), _start),
    EO_OP_FUNC(XML_SAX_BASE_ID(XML_SAX_BASE_SUB_ID_CHAR), _char),
    EO_OP_FUNC_SENTINEL
  };
  
  eo_class_funcs_set(klass, func_desc);
}

static const Eo_Op_Description op_desc[] = {
     EO_OP_DESCRIPTION_SENTINEL
};

static const Eo_Class_Description class_desc = {
     EO_VERSION,
     "Amin Machine Spec Document",
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(&AMIN_MACHINE_SPEC_DOCUMENT_BASE_ID, op_desc, AMIN_MACHINE_SPEC_DOCUMENT_SUB_ID_LAST),
     NULL,
     sizeof(Private_Data),
     _class_constructor,
     NULL
};

EO_DEFINE_CLASS(amin_machine_spec_document_class_get, &class_desc, AMIN_ELT, EO_BASE_CLASS, NULL);