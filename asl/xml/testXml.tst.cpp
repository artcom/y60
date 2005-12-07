//
// xml_test.cpp :
//

#include "Nodes.h"

#include <string>
#include <iterator>
#include <iostream>
#include <fstream>

#include <asl/Time.h>
#include <asl/file_functions.h>
#include <asl/numeric_functions.h>

//#include <pm/ptime.h>
//#define yrTime ptime
#include <assert.h>

//using namespace std;  // automatically added!

/*
 orig: 780-830
*/

/// read a complete file into a string
/*
std::string
file_as_string(const std::string& file_name)
{
	std::ifstream in_file(file_name.c_str());
	
	std::string result;
	std::vector<char> data_vec(65536);
	while (in_file) {
		in_file.read(data_vec.begin(),data_vec.size());
		result.append(data_vec.begin(),in_file.gcount());
	}
	return result;
}
*/

int dom_test() {
    {
        dom::Document root;
        
        root.appendChild(dom::Element("scene"));
        root.childNode("scene")->appendAttribute("version",1);
        dom::NodePtr info = root.childNode("scene")->appendChild(dom::Element("info"));
        
        info->appendChild(dom::Element("title"));
        info->childNode("title")->appendChild(dom::Text("Über große und kleine µ-Prozessoren"));
        
        dom::NodePtr actor = info->appendChild(dom::Element("actor"));
        actor->appendChild(dom::Element("name"))->appendChild(dom::Text("Pavel"));
        actor->appendChild(dom::Element("category"))->appendChild(dom::Text("amateur"));
        actor->appendChild(dom::Element("image"))->appendAttribute("url","http://www.artcom.de/images/pavel");
        
        dom::NodePtr model_urls = info->appendChild(dom::Element("model-urls"));
        model_urls->appendChild(dom::Element("model"))->appendAttribute("url","http://www.slashdot.org/");
        model_urls->appendChild(dom::Element("model"))->appendAttribute("url","http://www.heise.de/");
        model_urls->appendChild(dom::Element("model"))->appendAttribute("url","http://www.stadtplandienst.de/");
        model_urls->appendChild(dom::Element("model"))->appendAttribute("url","http://www.teleauskunft.com/");
        
        dom::NodePtr data = info->appendChild(dom::Element("data"));
        data->appendAttribute("url","http://www.artcom.de/model.iv");
        data->appendAttribute("duration","3:43");
        
        std::ofstream out("testdom.xml", std::ios::binary);
        out << root;
    }
    {
        dom::Node root2;
        std::ifstream f2("testdom.xml", std::ios::binary);
	    f2 >> root2;
        
        std::ofstream o2("testdom2.xml", std::ios::binary);
        o2 << root2;
        
        dom::NodePtr info = root2.childNode("scene")->childNode("info");
        dom::NodePtr model2 = info->childNode("model-urls")->childNode("model",2);
        if (model2->getAttribute("url")->nodeValue() == "http://www.stadtplandienst.de/")			
            std::cerr << root2;
        else
            std::cerr << "failure" << std::endl;
    }
    return 0;
    
}

#define TESTIF(x) if (!(x)) throw asl::Exception(JUST_FILE_LINE);
dom::Node     * ourTmpNode = 0;
dom::Document * ourTmpRoot = 0;
int dom_test2() {
    {
        dom::Document root;
        
        root.appendChild(dom::ProcessingInstruction());
        root("#comment") = "This is a artcom model file";

	std::cout << "DEBUG: dom_test2() adding content" << std::endl;

        root("scene")["version"]="1.0";
        const char * dp = root("scene")["version"].nodeValue().c_str();

	std::cout << "DEBUG: dom_test2() YES!!!" << std::endl;

        root("scene")("info")("title")("#text")="Über große und kleine µ-Prozessoren";
        root("scene")("info")("title")("#text")="Über große und kleine µ-Prozessoren";
        root("scene")("info")("actor")("name")("#text")="Pavel";
        root("scene")("info")("actor")("category")("#text")="amateur";
        root("scene")("info")("model-urls")("model",0)["url"]="http://www.slashdot.org/";
        root("scene")("info")("model-urls")("model",1)["url"]="http://www.heise.de/";
        root("scene")("info")("model-urls")("model",2)["url"]="http://www.stadtplandienst.de/";
        root("scene")("info")("model-urls")("model",3)["url"]="http://www.teleauskunft.com/";
        root("scene")("data")["url"] = "http://www.artcom.de/model.iv";
        root("scene")("data")["duration"] = "3:43";
        root("#comment") = "This is the end of a artcom model file";


	    std::cout << "DEBUG: dom_test2() done with adding content"
             << std::endl;

        try {
            std::string some_elem = "some-elem";
            std::string some_attr = "some-attr";
            std::string something = "something";
            root(some_elem)[some_attr] = something;
            root("some-elem")["some-attr"] = "something";
            std::cerr << "#### failed to detect second element in document"
              << std::endl;	
        }
        catch (dom::DomException& de) {
            std::cerr << "correct: refused second element in document: "
                << de  << std::endl;	
        }
        try {
            root("scene")("data")["duration?"] = "3:43";
            std::cerr << "#### failed to detect bad name"  << std::endl;	
        }
        catch (dom::DomException& de) {
            std::cerr << "correct: detected bad name: "<< de  << std::endl;	
        }
        
        std::ofstream o3("testdom3.xml", std::ios::binary);
        o3 << root;
    }
    {
        dom::Node root2;
        std::ifstream f2("testdom3.xml", std::ios::binary);
	    f2  >> root2;
        
        std::ofstream o4("testdom4.xml", std::ios::binary);
        o4 << root2;
        
        try {
            const dom::Node & scene = root2("scene");
            const dom::Node & version = scene["version"];

            const char * dp = root2("scene")["version"].nodeValue().c_str();

            TESTIF(root2("scene")["version"].nodeValue()=="1.0");
            TESTIF(root2("scene").getAttributeString("version") == "1.0");
            TESTIF(root2("scene")("info")("title")("#text").nodeValue()=="Über große und kleine µ-Prozessoren");
            TESTIF(root2("scene")("info")("title")("#text").nodeValue()=="Über große und kleine µ-Prozessoren");
            
            TESTIF(root2("scene")("info")("actor"));
            
            TESTIF(root2("scene")("info")("actor")("name")("#text").nodeValue()=="Pavel");
            TESTIF(root2("scene")("info")("actor")("category")("#text").nodeValue()=="amateur");
            TESTIF(root2("scene")("info")("model-urls")("model",0)["url"].nodeValue()=="http://www.slashdot.org/");
            TESTIF(root2("scene")("info")("model-urls")("model",0).getAttributeString("url")=="http://www.slashdot.org/");
            TESTIF(root2("scene")("info")("model-urls")("model",1)["url"].nodeValue()=="http://www.heise.de/");
            TESTIF(root2("scene")("info")("model-urls")("model",2)["url"].nodeValue()=="http://www.stadtplandienst.de/");
            TESTIF(root2("scene")("info")("model-urls")("model",3)["url"].nodeValue()=="http://www.teleauskunft.com/");
            TESTIF(root2("scene")("data")["url"].nodeValue()== "http://www.artcom.de/model.iv");
            TESTIF(root2("scene")("data")["url"].nodeValue()== "http://www.artcom.de/model.iv");
            TESTIF(root2("scene")("data").getAttributeString("url")== "http://www.artcom.de/model.iv");
            TESTIF(root2("scene")("data")["duration"].nodeValue()== "3:43");
            
            bool myExceptionCaught = false;
            try {
                root2("scene")("data").getAttributeString("badurl");
            } catch (dom::DomException) {
                myExceptionCaught = true;
            }

            TESTIF(myExceptionCaught);

            const dom::Node& cn = root2;
            int i = 0;
            while (cn("scene")("info")("model-urls")("model",i)) {
                std::cerr << cn("scene")("info")("model-urls")("model",i)["url"].nodeValue() << std::endl;
                ++i;
                TESTIF (i<10);
            }
            TESTIF(i==4);
            
            // keep in mind: you must not add a second element node to an element node
            // so this behaviour differs from the const behaviour
            TESTIF(root2("scene")("bla")["bla"].nodeValue()== "");
#if 0 
            
            TESTIF(cn("scene")("bla")["bla"].nodeValue()== "");
            TESTIF(cn("bla")("bla")["bla"].nodeValue()== "");
#endif    
        } 
        catch (dom::DomException& de) {
            std::cerr << "#### fatal failure:" << de << std::endl;	
        }
        catch (asl::Exception& e) {
            std::cerr << "#### data failure:" << e << std::endl;	
        }
    }
    return 0;
    
}

void testSerializer(const dom::Node & n, dom::Node & myRebinarizedDom) {
	std::cerr << "Starting binarizer test" << std::endl;
	asl::Block myBlock;
	asl::Time myStart;
	n.binarize(myBlock);
	asl::Time myFinish;
	double myDuration = myFinish-myStart;
	std::cerr << "Serialized to " << myBlock.size() << " bytes in " 
		<<  myDuration << "sec, MB/sec=" << myBlock.size()/myDuration/1024/1024<< std::endl;

	asl::Time myStart2;
	unsigned long myPos = myRebinarizedDom.debinarize(myBlock);
	if (myPos != myBlock.size()) {
		throw asl::Exception("debinarizer did not finish","testSerializer");
	}
	asl::Time myFinish2;
	double myDuration2 = myFinish2-myStart2;
	std::cerr << "Debinarized to " << myBlock.size() << " bytes in " 
		<<  myDuration2 << "sec, MB/sec=" << myBlock.size()/myDuration2/1024/1024<< std::endl;

}

int main(int argc, char* argv[])
{
    try {
	asl::initialize();
#if 1
//    bin_code_test(100);


//PORT DS: debug output
    std::cout << "DEBUG: dom_test()" << std::endl;
    dom_test();
    std::cout << "DEBUG: dom_test2()" << std::endl;
    dom_test2();
#endif
    //const char* file_name = argv[1]l;
    if (argc > 1 && std::string("profile") != argv[1]) {
        const char* file_name;
        file_name = argv[1];
        std::cout << asl::readFile(file_name) << std::endl;
        
        dom::Document dom;
        std::ifstream xml_file(file_name);
        
        if (xml_file >> dom)
            std::cerr << "parse ok" << std::endl;
        else
            std::cerr << "parse failed" << std::endl;
        std::cout << "------------- parsed until char " << dom.parsedUntil() 
            << ", total_size = " << dom.docSize() << std::endl;
        
        std::cout << dom;
        
        //std::cout << "press key & enter to continue" << std::endl;
        //char c; std::cin >> c;
    }
    else {
        asl::Time total_time = 0;

        std::vector<char*> file_name;
/*
--> orig
00:05.296
00:05.326
00:05.407
00:05.307
--> ptrpool
00:05.236
00:05.176
00:05.366
00:05.328
--> optimized read_if_string
00:03.936
00:03.905
00:03.905
--> Start/End Tag static strings
00:01.342
--> static entity strings
00:01.211
00:01.241
00:01.291
-->no-param-copy
00:01.252
00:01.201
00:01.252

new file:
 
00:02.423
*/

#define Nsingle_test        
#ifndef single_test        
        file_name.push_back("../../testdata/REC.xml");

        file_name.push_back("../../testdata/mini.xml");
        file_name.push_back("../../testdata/mini.xml");
        file_name.push_back("../../testdata/events.xml");
        file_name.push_back("../../testdata/events-many.xml");
        file_name.push_back("../../testdata/testdom.xml");
        file_name.push_back("../../testdata/testdom3.xml");
        file_name.push_back("../../testdata/formattest.xml");
        file_name.push_back("../../testdata/System Report.xml");
#endif        
#ifndef DEBUG_VARIANT        
		file_name.push_back("../../testdata/big.xml");
        file_name.push_back("../../testdata/all.xml");
#endif        
        /*
        file_name.push_back("\\Platform SDK\\Samples\\DataAccess\\TechArticles\\performance\\Code.xml");
        file_name.push_back("\\Platform SDK\\Samples\\Web\\Xml\\Xml_Validator\\WeatherReport.Xml");
        file_name.push_back("\\Platform SDK\\Samples\\Web\\Xml\\example_files\\Books.Xml");
        file_name.push_back("\\Platform SDK\\Samples\\Web\\Xml\\VbOrder\\Products.Xml");
        file_name.push_back("\\Platform SDK\\Samples\\Web\\Xml\\Stock_Sorter\\Stock-Sorter.Xml");
        */
        for (int i = 0; i < file_name.size(); ++i) {
#ifdef single_test        
            std::cout << asl::readFile(file_name[i]) << std::endl;
#endif
            std::cout << "Parsing file '" << file_name[i] << "'" << std::endl;
            
                asl::Time start_read1;
                std::string fs1(asl::readFile(file_name[i]));

                asl::Time start1;
                dom::Document dom(fs1);
                asl::Time ready1;
                double total_parse_time1 = ready1-start1;
                double total_read_time1 = start1-start_read1;
            
            std::cout << "------------- parsed until char " << dom.parsedUntil() 
                << ", total_size = " << dom.docSize() << std::endl;

            std::cout << "read time " << total_read_time1
                        << ", read rate (kb/s):" << fs1.size() / total_read_time1 / 1024.0 << std::endl;
            std::cout << "parse time " << total_parse_time1
                        << ", parse rate (kb/s):" << dom.parsedUntil() / total_parse_time1 / 1024.0 << std::endl;
            
            std::string file1;
            std::string file2;
            if (dom) {
#ifdef single_test        
                std::cerr << dom;
#endif
                {
                    asl::Time writeStart;
                    std::ofstream temp_out("tmpout.xml", std::ios::binary);
                    if (temp_out) {
                        if (temp_out << dom)
                            std::cerr << "Rewritten file to 'tmpout.xml'" << std::endl;;
                    }
                    asl::Time writeEnd;
                    double myDuration = writeEnd-writeStart;
                    unsigned long myFileSize =asl::getFileSize("tmpout.xml"); 
                    std::cerr << "Generated " <<  myFileSize << " xml bytes in " 
                        <<  myDuration << "sec, MB/sec=" << myFileSize/myDuration/1024/1024 << std::endl;
                }
                
                
                std::string fs(asl::readFile("tmpout.xml"));
                
                asl::Time start;
                dom::Document dom2(fs);
                asl::Time ready;
                total_time = total_time + ready-start;
                std::cerr << (ready-start) << "\n";
                if (dom2) {
                    std::cerr << "reparsed 'tmpout.xml' until char "<< dom2.parsedUntil() 
                        << ", total_size = " << dom2.docSize() << std::endl;
                    
                    {
                        std::ofstream of2("tmpout2.xml", std::ios::binary);
                        of2 << dom2;
                    }
                    
                    file1 = asl::readFile("tmpout.xml");
                    file2 = asl::readFile("tmpout2.xml");
                    //file2 = utils::as_string(dom2);
                    if ( file1 != file2 )
                    {
                        std::cerr << "#### ERROR: 1st and 2nd Representation differs! (tmpout.xml != tmpout2.xml)" << std::endl;
                        std::cerr << "file1.size() = " << file1.size()<< std::endl;
                        std::cerr << "file2.size() = " << file2.size() << std::endl;
                        for (int j = 0; j < asl::minimum(file1.size(),file2.size());++j) {
                            if (file1[j] == file2[j]) std::cerr << file1[j];
                            else std::cerr << "{"<< file1[j] << "|" << file2[j] << "}";
                        }			
                    } else {
                        std::cerr << "comparison succesful "<< std::endl;
						dom::Node myRebinarizedNode;
						testSerializer(dom2,myRebinarizedNode);
						std::string file3 = asl::as_string(myRebinarizedNode);
						if (file2 != file3) {
							std::cerr << "#### ERROR: (Serializer): 3rd and 2nd Representation differs!" << std::endl;
							std::cerr << "file3.size() = " << file3.size()<< std::endl;
							std::cerr << "file2.size() = " << file2.size() << std::endl;
							for (int j = 0; j < asl::minimum(file3.size(),file2.size());++j) {
								if (file3[j] == file2[j]) std::cerr << file3[j];
								else std::cerr << "{"<< file3[j] << "|" << file2[j] << "}";
							}			
						} else {
							std::cerr << "reserializion succesful, file3 size= "<<file3.size()<< std::endl;
						}
                    }
                    
                }
                else {
                    std::cerr << "failed to reparse 'tmpout.xml' until char "<< dom2.parsedUntil() 
                        << ", total_size = " << dom2.docSize() << std::endl;
                }
            }
            
#ifdef single_test        
            //std::cerr << "press key & enter to continue" << std::endl;
            //char c; std::cin >> c;
#endif        
        }
        std::cerr << "parse total_time ="; 
        std::cerr << total_time << "\n";
        //std::cerr << "press key & enter to continue" << std::endl;
        // char c; std::cin >> c;
    }
    } 
    catch (asl::Exception & myException) {
        std::cerr << myException << std::endl;
        return -1;
    }
    catch (std::exception & myException) {
        std::cerr << "std::exception:" << myException.what() << std::endl;
        return -1;
    }

    return 0;
}

