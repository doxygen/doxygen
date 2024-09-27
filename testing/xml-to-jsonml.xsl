<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:json="http://json.org/">

	<!--
	   XSLTJSON Lite v0.2 - Transform arbitrary XML to JSONML

	   Licensed under the new BSD License.
	   Copyright 2009, Bram Stein
	   All rights reserved.
	-->
	<xsl:output indent="no" omit-xml-declaration="yes" method="text" encoding="utf-8"/>
	<xsl:strip-space elements="*"/>
	<json:search name="string">
		<json:replace src="\" dst="\\"/>
		<json:replace src="&quot;" dst="\&quot;"/>
		<json:replace src="&#xA;" dst=""/>
		<json:replace src="&#xD;" dst="\r"/>
		<json:replace src="&#x9;" dst="\t"/>
		<json:replace src="\n" dst="\n"/>
		<json:replace src="\r" dst="\r"/>
		<json:replace src="\t" dst="\t"/>
	</json:search>
	<xsl:template match="node()">
		<xsl:text>[</xsl:text>
		<xsl:call-template name="encode-value">
			<xsl:with-param name="value" select="name()"/>
		</xsl:call-template>
		<xsl:if test="@*">
			<xsl:text>,</xsl:text>
			<xsl:text>{</xsl:text>
			<xsl:for-each select="@*">
				<xsl:apply-templates select="."/>
				<xsl:if test="position() != last()">
					<xsl:text>,</xsl:text>
				</xsl:if>
			</xsl:for-each>
			<xsl:text>}</xsl:text>
		</xsl:if>
		<xsl:if test="node()">
			<xsl:text>,</xsl:text>
			<xsl:for-each select="node()">
				<xsl:apply-templates select="."/>
				<xsl:if test="position() != last()">
					<xsl:text>,</xsl:text>
				</xsl:if>
			</xsl:for-each>
		</xsl:if>
		<xsl:text>]</xsl:text>
	</xsl:template>
	<xsl:template match="text()" priority="10">
		<xsl:call-template name="encode-value">
			<xsl:with-param name="value" select="."/>
		</xsl:call-template>
	</xsl:template>
	<xsl:template match="@*">
		<xsl:call-template name="encode-value">
			<xsl:with-param name="value" select="name()"/>
		</xsl:call-template>
		<xsl:text>:</xsl:text>
		<xsl:call-template name="encode-value">
			<xsl:with-param name="value" select="."/>
		</xsl:call-template>
	</xsl:template>
	<xsl:template name="replace-string">
		<xsl:param name="input"/>
		<xsl:param name="src"/>
		<xsl:param name="dst"/>
		<xsl:choose>
			<xsl:when test="contains($input, $src)">
				<xsl:value-of select="concat(substring-before($input, $src), $dst)"/>
				<xsl:call-template name="replace-string">
					<xsl:with-param name="input" select="substring-after($input, $src)"/>
					<xsl:with-param name="src" select="$src"/>
					<xsl:with-param name="dst" select="$dst"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$input"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template name="encode">
		<xsl:param name="input"/>
		<xsl:param name="index">1</xsl:param>
		<xsl:variable name="text">
			<xsl:call-template name="replace-string">
				<xsl:with-param name="input" select="$input"/>
				<xsl:with-param name="src" select="document('')//json:search/json:replace[$index]/@src"/>
				<xsl:with-param name="dst" select="document('')//json:search/json:replace[$index]/@dst"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$index &lt; count(document('')//json:search/json:replace)">
				<xsl:call-template name="encode">
					<xsl:with-param name="input" select="$text"/>
					<xsl:with-param name="index" select="$index + 1"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$text"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template name="encode-value">
		<xsl:param name="value"/>
		<xsl:choose>
			<xsl:when test="(normalize-space($value) != $value or string(number($value)) = 'NaN' or (substring($value , string-length($value), 1) = '.') or (substring($value, 1, 1) = '0') and not($value = '0')) and not($value = 'false') and not($value = 'true') and not($value = 'null')">
				<xsl:text>"</xsl:text>
				<xsl:call-template name="encode">
					<xsl:with-param name="input" select="$value"/>
				</xsl:call-template>
				<xsl:text>"</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$value"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
