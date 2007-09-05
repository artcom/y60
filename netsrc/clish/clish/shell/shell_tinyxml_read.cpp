////////////////////////////////////////
// shell_tinyxml_read.cpp
// 
// This file implements the means to read an XML encoded file and populate the 
// CLI tree based on the contents.
////////////////////////////////////////
extern "C"
{
#include "private.h"
#include "lub/string.h"
}
/*lint +libh(tinyxml/tinyxml.h) Add this to the library file list */
#include "tinyxml/tinyxml.h"

#include <string.h>
#include <assert.h>

typedef void (PROCESS_FN)(clish_shell_t *instance, 
                         TiXmlElement  *element,
                         void          *parent);

// Define a control block for handling the decode of an XML file
typedef struct clish_xml_cb_s clish_xml_cb_t;
struct clish_xml_cb_s
{
    const char *element;
    PROCESS_FN *handler;
};

// forward declare the handler functions
static PROCESS_FN 
    process_clish_module,
    process_startup,
    process_view,
    process_command,
    process_param,
    process_action,
    process_ptype,
    process_overview,
    process_detail;

static clish_xml_cb_t xml_elements[]=
{
    {"CLISH_MODULE", process_clish_module},
    {"STARTUP"     , process_startup     },
    {"VIEW"        , process_view        },
    {"COMMAND"     , process_command     },
    {"PARAM"       , process_param       },
    {"ACTION"      , process_action      },
    {"PTYPE"       , process_ptype       },
    {"OVERVIEW"    , process_overview    },
    {"DETAIL"      , process_detail      },
    {NULL          , NULL                }
};
///////////////////////////////////////
// This function reads an element from the XML stream and processes it.
///////////////////////////////////////
static void
process_node(clish_shell_t *shell,
             TiXmlNode     *node,
             void          *parent)
{
    switch(node->Type())
    {
    case TiXmlNode::DOCUMENT:
        break;
    case TiXmlNode::ELEMENT:
        clish_xml_cb_t *cb;
        for(cb = &xml_elements[0];
            cb->element;
            cb++)
        {
            if(0 == strcmp(node->Value(),cb->element))
            {               
                TiXmlElement *element = (TiXmlElement*)node;
#ifdef DEBUG
                printf("NODE:");
                element->Print(stdout,0);
                printf("\n***\n");
#endif
                // process the elements at this level
                cb->handler(shell,element,parent);
                break;
            }
        }
        break;
    case TiXmlNode::COMMENT:
    case TiXmlNode::TEXT:
    case TiXmlNode::DECLARATION:
    case TiXmlNode::TYPECOUNT:
    case TiXmlNode::UNKNOWN:
    default:
        break;
    }
}
///////////////////////////////////////
static void 
process_children(clish_shell_t *shell,
                 TiXmlElement  *element,
                 void          *parent=NULL)
{
    for(TiXmlNode *node = element->FirstChild();
        NULL           != node;
        node            = element->IterateChildren(node))
    {
        // Now deal with all the contained elements
        process_node(shell,node,parent);
    }
}
///////////////////////////////////////
static void
process_clish_module(clish_shell_t *shell,
                     TiXmlElement  *element,
                     void          *)
{
    // create the global view
    if(NULL == shell->global)
    {
        shell->global = clish_shell_find_create_view(shell,"global","");
    }
    process_children(shell,element,shell->global);
}
///////////////////////////////////////
static void
process_view(clish_shell_t *shell,
             TiXmlElement  *element,
             void          *)
{
    clish_view_t *view;
    const char   *name   = element->Attribute("name");  
    const char   *prompt = element->Attribute("prompt");

    // re-use a view if it already exists
    view = clish_shell_find_create_view(shell,name,prompt);

    process_children(shell,element,view);
}
///////////////////////////////////////
static void
process_ptype(clish_shell_t *shell,
              TiXmlElement  *element,
              void          *)
{
    clish_ptype_method_e     method;
    clish_ptype_preprocess_e preprocess;
    clish_ptype_t *ptype;
    const char    *name            = element->Attribute("name");
    const char    *help            = element->Attribute("help");
    const char    *pattern         = element->Attribute("pattern");
    const char    *method_name     = element->Attribute("method");
    const char    *preprocess_name = element->Attribute("preprocess");
    assert(name);
    assert(pattern);
    method     = clish_ptype_method_resolve(method_name);
    preprocess = clish_ptype_preprocess_resolve(preprocess_name);

    ptype = clish_shell_find_create_ptype(shell,
                                          name,
                                          help,
                                          pattern,
                                          method,
                                          preprocess);
    assert(ptype);
}
///////////////////////////////////////
static void
process_overview(clish_shell_t *shell,
                 TiXmlElement  *element,
                 void          *)
{
    // read the following text element
    TiXmlNode *text = element->FirstChild();

    if(NULL != text)
    {
        assert(TiXmlNode::TEXT == text->Type());
        // set the overview text for this view
        assert(NULL == shell->overview);
        // store the overview
        shell->overview = lub_string_dup(text->Value());
    }
}
////////////////////////////////////////
static void
process_command(clish_shell_t *shell,
                TiXmlElement  *element,
                void          *parent)
{
    clish_view_t    *v       = (clish_view_t*)parent;
    clish_command_t *cmd     = NULL;
    const char      *access  = element->Attribute("access");
    bool             allowed = true;
    
    if(NULL != access)
    {
        allowed = false; // err on the side of caution
        if(shell->client_hooks->access_fn)
        {
            // get the client to authenticate
            allowed = shell->client_hooks->access_fn(shell,access) ? true : false;
        }
    }
    
    if(allowed)
    {
        const char *name         = element->Attribute("name");
        const char *help         = element->Attribute("help");
        const char *view         = element->Attribute("view");
        const char *viewid       = element->Attribute("viewid");
        const char *escape_chars = element->Attribute("escape_chars");
        const char *args_name    = element->Attribute("args");
        const char *args_help    = element->Attribute("args_help");
        
        clish_command_t *old = clish_view_find_command(v,name);

        // check this command doesn't already exist
        if(NULL != old)
        {
            // flag the duplication then ignore further definition
            printf("DUPLICATE COMMAND: %s\n",clish_command__get_name(old));
        }
        else
        {
            assert(name);
            assert(help);
            /* create a command */
            cmd = clish_view_new_command(v,name,help);
            assert(cmd);
            if(NULL != escape_chars)
            {
                /* define some specialist escape characters */
                clish_command__set_escape_chars(cmd,escape_chars);
            }
            if(NULL != args_name)
            {
                /* define a "rest of line" argument */
                clish_param_t *param;
                
                assert(NULL != args_help);
                param = clish_param_new(args_name,args_help,NULL);
                
                clish_command__set_args(cmd,param);
            }
            // define the view which this command changes to
            if(NULL != view)
            {
                clish_view_t *next = clish_shell_find_create_view(shell,view,NULL);

                // reference the next view
                clish_command__set_view(cmd,next);
            }
            // define the view id which this command changes to
            if(NULL != viewid)
            {
                clish_command__set_viewid(cmd,viewid);
            }
            process_children(shell,element,cmd);
        }
    }
}
///////////////////////////////////////
static void
process_startup(clish_shell_t *shell,
                TiXmlElement  *element,
                void          *parent)
{
    clish_view_t    *v       = (clish_view_t*)parent;
    clish_command_t *cmd     = NULL;
    const char      *view   = element->Attribute("view");
    const char      *viewid = element->Attribute("viewid");

    assert(NULL == shell->startup);
    assert(view);
        
    /* create a command with NULL help */
    cmd = clish_view_new_command(v,"startup",NULL);

    // define the view which this command changes to
    clish_view_t *next = clish_shell_find_create_view(shell,view,NULL);
    // reference the next view
    clish_command__set_view(cmd,next);

    // define the view id which this command changes to
    if(NULL != viewid)
    {
        clish_command__set_viewid(cmd,viewid);
    }
    
    // remember this command 
    shell->startup = cmd;
    
    process_children(shell,element,cmd);
}
///////////////////////////////////////
static void
process_param(clish_shell_t *shell,
              TiXmlElement  *element,
              void          *parent)
{
    clish_command_t *cmd = (clish_command_t *)parent;

    if(NULL != cmd)
    {
        assert(cmd != shell->startup);
        const char          *name   = element->Attribute("name");
        const char          *help   = element->Attribute("help");
        const char          *ptype  = element->Attribute("ptype");
        const char          *prefix = element->Attribute("prefix");
        const char          *defval = element->Attribute("default");
        clish_param_t       *param;
        clish_ptype_t *tmp = NULL;
        
        // create a command
        assert(name);
        assert(help);
        assert(ptype);
        if(ptype && *ptype)
        {
            tmp = clish_shell_find_create_ptype(shell,ptype,
                                                NULL,NULL,
                                                CLISH_PTYPE_REGEXP,
                                                CLISH_PTYPE_NONE);
            assert(tmp);
        }
        param = clish_param_new(name,help,tmp);
        if(NULL != prefix)
        {
            clish_param__set_prefix(param,prefix);
        }
        if(NULL != defval)
        {
            clish_param__set_default(param,defval);
        }
        // add the parameter to the command
        clish_command_insert_param(cmd,param);
    }
}
////////////////////////////////////////
static void
process_action(clish_shell_t *shell,
               TiXmlElement  *element,
               void          *parent)
{
    clish_command_t *cmd = (clish_command_t *)parent;

    if(NULL != cmd)
    {
        // read the following text element
        TiXmlNode  *text    = element->FirstChild();
        const char *builtin = element->Attribute("builtin");

        if(NULL != text)
        {
            assert(TiXmlNode::TEXT == text->Type());
            // store the action
            clish_command__set_action(cmd,text->Value());
        }
        if(NULL != builtin)
        {
            // store the action
            clish_command__set_builtin(cmd,builtin);
        }
    }
}
////////////////////////////////////////
static void
process_detail(clish_shell_t *shell,
               TiXmlElement  *element,
               void          *parent)
{
    clish_command_t *cmd = (clish_command_t *)parent;

    // read the following text element
    TiXmlNode *text = element->FirstChild();

    if((NULL != cmd) && (NULL != text))
    {
        assert(TiXmlNode::TEXT == text->Type());
        // store the action
        clish_command__set_detail(cmd,text->Value());
    }
}
///////////////////////////////////////
int 
clish_shell_xml_read(clish_shell_t *shell,
                     const char    *filename) 
{
    int           ret = -1;
    TiXmlDocument doc;
    
    // keep the white space 
    TiXmlBase::SetCondenseWhiteSpace(false);
    
    if(doc.LoadFile(filename))
    {
        TiXmlNode *child = 0;
        while( (child = doc.IterateChildren(child)) )
        {
            process_node(shell,child,NULL);
        }
        ret = 0;
    }
    else 
    {
        printf("Unable to open %s\n", filename);
    }
    return ret;
}
///////////////////////////////////////
