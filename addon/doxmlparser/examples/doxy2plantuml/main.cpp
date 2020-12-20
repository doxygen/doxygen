
#include <doxmlintf.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <stack>

#include <cstring>


int main(int argc, char **argv) {
    std::string xml_output_dir = "";
    std::string plantuml_filename = "output.xml";

	if (argc != 3)
	{
		printf("Usage: %s xml_output_dir plantuml_filename\n", argv[0]);
		exit(1);
	}
	else
	{
		xml_output_dir = std::string(argv[1]);
		plantuml_filename = std::string(argv[2]);
	}


    std::string xml_dir = (xml_output_dir + "/xml/");

    IDoxygen *doxygen = createObjectModel();
    if (!doxygen->readXMLDir(xml_dir.c_str())) {
        throw std::runtime_error(xml_dir + " not found");
    }


    std::stack<IGraph*> collaboration_graph_stack;
    std::stack<IGraph*> inheritance_graph_stack;

    std::ofstream outfile(plantuml_filename);
    outfile << "@startuml" << std::endl;


    // check if the program is C++
    std::fstream index_xml;
    index_xml.open(xml_dir + "index.xml");

    std::string line = "";
    while (!index_xml.eof()) {
        line = "";
        getline(index_xml, line);
        if (line.find("cpp")) {
            outfile << "set namespaceSeparator ::" << std::endl;
            break;
        }
    }
    index_xml.close();


    // For each compound.
    ICompoundIterator *compound_iter = doxygen->compounds();
    while (compound_iter->current() != nullptr) {
        ICompound *compound = compound_iter->current();

        // For each namespace
        if (compound->kind() == ICompound::CompoundKind::Namespace) {
            INamespace *namespace_compound = dynamic_cast<INamespace *>(compound);
            outfile << "namespace " << namespace_compound->name()->utf8() << " {" << std::endl;

            // For each class within the namespace.
            ICompoundIterator* ns_class_iter = namespace_compound->nestedCompounds();
            while (ns_class_iter->current() != nullptr) {
                ICompound* ns_class_compound = ns_class_iter->current();
                if (ns_class_iter->current()->kind() == ICompound::CompoundKind::Class) {
                    IClass* ns_class = dynamic_cast<IClass*>(ns_class_compound);

                    // save the graphs for later processing
                    if (ns_class->collaborationGraph() != nullptr) {
                        collaboration_graph_stack.push(ns_class->collaborationGraph());
                    }
                    if (ns_class->inheritanceGraph() != nullptr) {
                        inheritance_graph_stack.push(ns_class->inheritanceGraph());
                    }

                    outfile << "class " << ns_class->name()->utf8() << "{" << std::endl;

                    // for all sections
                    ISectionIterator* section_iter = ns_class->sections();
                    while (section_iter->current() != nullptr) {
                        IMemberIterator* member_iter = section_iter->current()->members();

                        // for all members
                        while (member_iter->current() != nullptr) {
                            IMember::MemberKind member_kind = member_iter->current()->kind();

                            IMember* member_ptr = member_iter->current();
                            const char * member_name = member_ptr->name()->utf8();
                            const char * member_protection = member_ptr->protection()->utf8();

                            if (strcmp(member_protection, "public") == 0) {
                                outfile << "+";
                            } else if (strcmp(member_protection, "protected") == 0) {
                                outfile << "-";
                            } else if (strcmp(member_protection, "private") == 0) {
                                outfile << "#";
                            }

                            if (member_kind == IMember::MemberKind::Function) {
                                const char * args_string = member_ptr->argsstring()->utf8();

                                outfile << member_name << args_string << std::endl;
                            } else if (member_kind == IMember::MemberKind::Variable) {
                                outfile << member_name << " : " << member_ptr->typeString()->utf8() << std::endl;
                            }

                            member_iter->toNext();
                        }
                        member_iter->release();
                        section_iter->toNext();
                    }
                    section_iter->release();

                    outfile << "}" << std::endl;
                }
                ns_class_iter->toNext();
            }
            ns_class_iter->release();

            outfile << "}" << std::endl;
        }

        compound_iter->toNext();
    }
    compound_iter->release();


    // to stop classes being processed more than once.
    std::map<std::string, bool> processed_class_name_map;

    //
    // Write all the inhertiances out
    //
    while (!inheritance_graph_stack.empty()) {
        IGraph* graph_starting_node = inheritance_graph_stack.top();

        INodeIterator* iter = graph_starting_node->nodes();
        while (iter->current() != nullptr) {
            INode* curr_node = iter->current();

            const char * curr_node_name = curr_node->label()->utf8();

            // skip already processed classes
            if (processed_class_name_map.find(curr_node_name) != processed_class_name_map.end()) {
                iter->toNext();
                continue;
            }
            processed_class_name_map[curr_node_name] = true;

            IChildNodeIterator* child_node_iterator = curr_node->children();
            while (child_node_iterator->current() != nullptr) {
                IChildNode* child_node = child_node_iterator->current();
                outfile << curr_node_name << " --|> " << child_node->node()->label()->utf8() << std::endl;
                child_node_iterator->toNext();
            }
            child_node_iterator->release();

            iter->toNext();
        }
        iter->release();

        inheritance_graph_stack.pop();
    }

    processed_class_name_map.clear();

    //
    // Write all the collaborations out
    //
    while (!collaboration_graph_stack.empty()) {
        IGraph* graph_starting_point = collaboration_graph_stack.top();

        INodeIterator* iter = graph_starting_point->nodes();
        while (iter->current() != nullptr) {
            INode* curr_node = iter->current();

            const char * curr_node_name = curr_node->label()->utf8();

            // skip already processed classes
            if (processed_class_name_map.find(curr_node_name) != processed_class_name_map.end()) {
                iter->toNext();
                continue;
            }
            processed_class_name_map[curr_node_name] = true;

            IChildNodeIterator* child_node_iterator = curr_node->children();
            while (child_node_iterator->current() != nullptr) {
                IChildNode* child_node = child_node_iterator->current();

                // for some reason inherited relationships are in the collaborations
                if (child_node->relation() != IChildNode::Usage) {
                    child_node_iterator->toNext();
                    continue;
                }
                outfile << curr_node->label()->utf8() << " --> " << child_node->node()->label()->utf8() << std::endl;
                child_node_iterator->toNext();
            }
            child_node_iterator->release();

            iter->toNext();
        }
        iter->release();

        collaboration_graph_stack.pop();
    }


    outfile << "@enduml" << std::endl;

    doxygen->release();

    return 0;
}
