#include <iostream>
#include "gnl_filesystem.h"

#define GNL_JSON_IMPLEMENT
#include "gnl_json.h"

using namespace std;

int main()
{


    std::string raw =

R"delimiter(

    {
        default_font:"Vera",
        default_font_size:14,
        default_font_colour:[1.0, 1.0, 1.0, 1.0],

        ListItem: {
            background_image:"none",
            background_image_hover:"listitem_hover",
            background_image_down:"listitem_down",
            text_position:"left"
        },


        Button : {
            background_image:"button",
            background_image_down:"button_down",
            background_image_hover:"button_hover",
            border_thickness:[16, 16],
            coord_border_thickness:[16, 16]
        },

        Slider : {
            min_size:[32, 32],

            background : {
                background_image:"sliderbar",
                border_thickness:[20, 5],
                coord_border_thickness:[20, 5],
                ignore_events:true,
                max_size:[9999 ,16],
                min_size:[64 ,16]
            },

            handle : {
                background_image:"sliderhandle",
                background_image_down:"sliderhandle_down",
                background_image_hover:"sliderhandle_hover",
                colour:[1.0, 1.0 ,1.0 ,1.0],
                size:[32, 32],
                min_size:[32, 32],
                max_size:[32 ,32],
                border_thickness:[0, 0],
                coord_border_thickness:[0, 0]
            }
        },

        ProgressBar : {
            min_size:[16, 16],
            max_size:[9999 ,40],

            background : {
                background_image:"progressbar_background",
                border_thickness:[8 ,8],
                coord_border_thickness:[16, 16],
                ignore_events:true
            },

            bar : {
                background_image:"progressbar_fill",
                border_thickness:[8, 8],
                coord_border_thickness:[16, 16],
                colour:[1.0, 1.0, 1.0, 1.0],
                min_size:[16, 16],
                ignore_events:true
            },

            label : {
                colour:[0.2, 0.2, 1.0, 1.0]
            }
        },

        CheckBox : {
            text:"Check Box",
            button_position:"left",

            button : {
                min_size:[20,20],
                max_size:[20,20],
                size:[20,20],
                border_thickness:[0,0],
                coord_border_thickness:[0,0],

                background_image:"checkbox",
                background_image_down:"checkbox_down",
                background_image_hover:"checkbox_hover"
            },

            label : {
                colour:[1.0,1.0,1.0,1.0]
            }
        },

        Textbox : {
            min_size:[40, 40],
            max_size:[99999, 40],
            background_image:"textbox",
            border_thickness:[20, 20],
            coord_border_thickness:[20, 20],

            cursor : {
                background_image:"cursor",
                visible:false
            },

            label : {
                colour:[1.0,1.0,1.0,1.0],
                text_position:"left"
            }
        },

        ListItem : {
            background_image:"none",
            background_image_hover:"listitem_hover",
            background_image_down:"listitem_down",
            border_thickness:[0 , 0],
            coord_border_thickness:[0 , 0],
            size:[150, 20],
            max_size:[9999, 20],
            min_size:[0, 20],
            text_position:"left"
        },

        List : {
            background_image:"textbox",
            border_thickness:[8 ,8],
            coord_border_thickness:[8 ,8],

            label : {
                colour:[1.0 ,1.0, 1.0, 1.0],
                text_position:"center"
            }

        },

        ComboItem : {
            background_image:"none",
            background_image_hover:"listitem_hover",
            background_image_down:"listitem_down",
            border_thickness:[0, 0],
            coord_border_thickness:[0 ,0],

            size:[150, 20],
            max_size:[99999, 20],
            min_size:[0, 20],

            text_position:"left"
        },

        Combobox : {
            min_size:[32, 32],
            max_size:[999999, 32],

            background_image:"combobox",
            border_thickness:[8, 8],
            coord_border_thickness:[8, 8],


            label : {
                text_position:"left"
            },

            button : {
                background_image:"combobox_icon",
                background_image_down:"combobox_icon_down",
                background_image_hover:"combobox_icon_hover",
                max_size:[32, 32],
                min_size:[32, 32]
            },


            dropdown : {
                max_size:[10000, 200],
                border_thickness:[0, 0]
            }
        },

        Window : {
            min_size:[32 ,32],
            background_image:"windowpanel",
            border_thickness:[7 ,7],
            coord_border_thickness:[16 ,16],
            background_colour:[0.25 ,0.25 ,1.0, 1.0],


            titlebar : {
                border_thickness:[7, 7],
                coord_border_thickness:[16, 16],
                max_size:[9999 ,32],
                min_size:[40   ,32],
                background_colour:[0.25 ,0.25, 1.0, 1.0],
                background_image:"titlebar",
                background_image_down:"titlebar",
                background_image_hover:"titlebar"
            },

            closebutton : {
                min_size:[20, 20],
                max_size:[20, 20],
                size:[20, 20],

                border_thickness:[0, 0],
                coord_border_thickness:[0, 0],
                background_image:"windowclosebutton",
                background_image_down:"windowclosebutton_down",
                background_image_hover:"windowclosebutton_hover"
            }
        },


        VerticalScrollBar : {
            max_size:[16,1000],
            min_size:[16,0],

            forward : {
                background_image:"scrolldown",
                background_image_down:"scrolldown_down",
                background_image_hover:"scrolldown_hover",
                max_size:[16,16],
                min_size:[16,16],
                border_thickness:[0,0],
                coord_border_thickness:[0,0]
            },

            back : {
                background_image:"scrollup",
                background_image_down:"scrollup_down",
                background_image_hover:"scrollup_hover",
                max_size:[16,16],
                min_size:[16,16],
                border_thickness:[0,0],
                coord_border_thickness:[0,0]
            },

            handle : {
                background_image:"scrollhandle",
                background_image_down:"scrollhandle_down",
                background_image_hover:"scrollhandle_hover",
                max_size:[16,16],
                min_size:[16,16],
                border_thickness:[0,0],
                coord_border_thickness:[0,0]
            },

            bar : {
                background_image:"vscrollbar",
                max_size:[16,10000],
                min_size:[0,0]
            }
        },

        HorizontalScrollBar : {
            max_size:[1000,16],
            min_size:[16,0],

            forward : {
                background_image:"scrollright",
                background_image_down:"scrollright_down",
                background_image_hover:"scrollright_hover",
                max_size:[16,16],
                min_size:[16,16]
            },

            back : {
                background_image:"scrollleft",
                background_image_down:"scrollleft_down",
                background_image_hover:"scrollleft_hover",
                max_size:[16,16],
                min_size:[16,16]
            },

            handle : {
                background_image:"scrollhandle",
                background_image_down:"scrollhandle_down",
                background_image_hover:"scrollhandle_hover",
                max_size:[16,16],
                min_size:[16,16]
            },

            bar : {
                background_image:"hscrollbar",
                max_size:[10000,16],
                min_size:[0,0]
            }
        },

        Ztest : [3,2,3,2,1,4,5,2]

    }

}

)delimiter";


    std::istringstream S(raw);


    gnl::json::Value O;
    O.parse(S);


    O["HorizontalScrollBar"][12] = 132.0f;
    for(auto a : O.getValueMap() )
    {
        std::cout << a.first << ": " << a.second.type() << std::endl;
    }
    return 0;
    gnl::json::Value T;
    T["firstname"] = "gavin";
    T["age"] = 30;
    T["family"][0] = "Glenn";
    T["family"][1] = "May";
    T["family"][2] = "Calvin";


    for(auto a : T.getValueMap() )
    {
        std::cout << a.first << ": " << a.second.type() << std::endl;
    }

    //std::cout << "(" << k << ")  next= (" << (char)S.get() << ")" << std::endl;
    return 0;
    {
        gnl::Path P("c:\\home\\commander\\test.png");
        std::cout << "FileName : " << P.getFileName()          << std::endl;
        std::cout << "BaseName : " << P.getFileBasename()      << std::endl;
        std::cout << "Extension: " << P.getFileExtension()     << std::endl;
        std::cout << "Parent   : " << P.getParent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    {
        gnl::Path P("/home/commander/test.png");
        std::cout << "FileName : " << P.getFileName()          << std::endl;
        std::cout << "BaseName : " << P.getFileBasename()      << std::endl;
        std::cout << "Extension: " << P.getFileExtension()     << std::endl;
        std::cout << "Parent   : " << P.getParent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    {
        gnl::Path P("home/commander/test.png");
        std::cout << "FileName : " << P.getFileName()          << std::endl;
        std::cout << "BaseName : " << P.getFileBasename()      << std::endl;
        std::cout << "Extension: " << P.getFileExtension()     << std::endl;
        std::cout << "Parent   : " << P.getParent().toString() << std::endl;
        std::cout << "Full Path: " << P.toString()             << std::endl;
        std::cout << "Absolute : " << P.isAbsolute()           << std::endl;
        cout << "==========" << endl;
    }
    return 0;
}

#define GNL_IMPLEMENTATION
#include "gnl_filesystem.h"
#undef GNL_IMPLEMENT
