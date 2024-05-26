for file in $(ls *.xsd | grep -v '^xml\.xsd$'); do
  initial=${file:0:1}
  remainder=${file:1}
  cat <<--
	QVariantMap Doxml::parse${initial^^}${remainder%.xsd}(QXmlStreamReader &xml) const
	{
	    /// \todo Implement Doxml::parse${initial^^}${remainder%.xsd}().
	    Q_UNUSED(xml)
	    return {};
	}

	-
  sed -Ene "$(cat <<--
	s|.*xsd:complexType +name="([^"]+)".*|\
QVariantMap Doxml::parse${initial^^}${remainder%.xsd}_\1(QXmlStreamReader \&xml) const\n{\n\
    /// \\\todo Implement Doxml::parse${initial^^}${remainder%.xsd}_\1().\n\
    Q_UNUSED(xml)\n\
    return {};\n\
}\n\
|p
	-
	)" "${file}"
done
