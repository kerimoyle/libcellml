#include "gtest/gtest.h"

#include <libcellml>


TEST(Model, serialise) {
    const std::string e = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<model xmlns=\"http://www.cellml.org/cellml/1.2#\"></model>";
    libcellml::Model m = libcellml::Model();
    std::string a = m.serialise(libcellml::CELLML_FORMAT_XML);

   EXPECT_EQ(e, a);
}

TEST(Model, name) {
    std::string n = "name";
    const std::string e = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<model xmlns=\"http://www.cellml.org/cellml/1.2#\" name=\"" + n + "\"></model>";

    libcellml::Model m = libcellml::Model();
    m.setName(n);

    EXPECT_EQ("name", m.getName());

    EXPECT_EQ(e, m.serialise(libcellml::CELLML_FORMAT_XML));
}

TEST(Model, unset_name) {
    std::string n = "name";
    const std::string eName = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<model xmlns=\"http://www.cellml.org/cellml/1.2#\" name=\"" + n + "\"></model>";
    const std::string e = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<model xmlns=\"http://www.cellml.org/cellml/1.2#\"></model>";

    libcellml::Model m = libcellml::Model();
    m.setName(n);
    EXPECT_EQ("name", m.getName());
    EXPECT_EQ(eName, m.serialise(libcellml::CELLML_FORMAT_XML));

    m.setName("");
    EXPECT_EQ("", m.getName());
    EXPECT_EQ(e, m.serialise(libcellml::CELLML_FORMAT_XML));
}

TEST(Model, invalid_name) {
    std::string in = "invalid name";
    const std::string e = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<model xmlns=\"http://www.cellml.org/cellml/1.2#\" name=\"" + in + "\"></model>";

    libcellml::Model m = libcellml::Model();
    m.setName(in);

    EXPECT_EQ("invalid name", m.getName());

    EXPECT_EQ(e, m.serialise(libcellml::CELLML_FORMAT_XML));
}
