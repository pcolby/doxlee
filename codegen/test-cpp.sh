for file in $(ls *.xsd | grep -v '^xml\.xsd$'); do
  initial=${file:0:1}
  remainder=${file:1}
  cat <<--
	void TestDoxml::parse${initial^^}${remainder%.xsd}_data()
	{
	}

	void TestDoxml::parse${initial^^}${remainder%.xsd}()
	{
	    /// \todo Implement TestDoxml::parse${initial^^}${remainder%.xsd}().
	    QXmlStreamReader xml;
	    doxlee::Doxml doxml(QString{});
	    QCOMPARE(doxml.parse${initial^^}${remainder%.xsd}(xml), QVariantMap{});
	}

	-
  sed -Ene "$(cat <<--
	s|.*xsd:complexType +name="([^"]+)".*|\
void TestDoxml::parse${initial^^}${remainder%.xsd}_\1_data()\n{\n}\n\n\
void TestDoxml::parse${initial^^}${remainder%.xsd}_\1()\n{\n\
    /// \\\todo Implement TestDoxml::parse${initial^^}${remainder%.xsd}_\1().\n\
    QXmlStreamReader xml;\n\
    doxlee::Doxml doxml(QString{});\n\
    QCOMPARE(doxml.parse${initial^^}${remainder%.xsd}_\1(xml), QVariantMap{});\n\
}\n\
|p
	-
	)" "${file}"
done
