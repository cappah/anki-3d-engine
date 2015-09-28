// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "Exporter.h"
#include <iostream>

//==============================================================================
void Exporter::exportMaterial(
	const aiMaterial& mtl,
	uint32_t instances) const
{
	std::string diffTex;
	std::string normTex;
	std::string specColTex;
	std::string shininessTex;
	std::string dispTex;
	std::string emissiveTex;

	aiString path;

	std::string name = getMaterialName(mtl, instances);
	LOGI("Exporting material %s", name.c_str());

	// Diffuse texture
	if(mtl.GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		if(mtl.GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			diffTex = getFilename(path.C_Str());
		}
		else
		{
			ERROR("Failed to retrieve texture");
		}
	}

	// Normal texture
	if(mtl.GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		if(mtl.GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
		{
			normTex = getFilename(path.C_Str());
		}
		else
		{
			ERROR("Failed to retrieve texture");
		}
	}

	// Specular color
	if(mtl.GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		if(mtl.GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
		{
			specColTex = getFilename(path.C_Str());
		}
		else
		{
			ERROR("Failed to retrieve texture");
		}
	}

	// Shininess color
	if(mtl.GetTextureCount(aiTextureType_SHININESS) > 0)
	{
		if(mtl.GetTexture(aiTextureType_SHININESS, 0, &path) == AI_SUCCESS)
		{
			shininessTex = getFilename(path.C_Str());
		}
		else
		{
			ERROR("Failed to retrieve texture");
		}
	}

	// Height texture
	if(mtl.GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
	{
		if(mtl.GetTexture(aiTextureType_DISPLACEMENT, 0, &path) == AI_SUCCESS)
		{
			dispTex = getFilename(path.C_Str());
		}
		else
		{
			ERROR("Failed to retrieve texture");
		}
	}

	// Emissive texture
	if(mtl.GetTextureCount(aiTextureType_EMISSIVE) > 0)
	{
		if(mtl.GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS)
		{
			emissiveTex = getFilename(path.C_Str());
		}
		else
		{
			ERROR("Failed to retrieve texture");
		}
	}

	// Write file
	static const char* diffNormSpecFragTemplate =
#include "templates/diffNormSpecFrag.h"
		;
	static const char* simpleVertTemplate =
#include "templates/simpleVert.h"
		;
	static const char* tessVertTemplate =
#include "templates/tessVert.h"
		;

	static const char* readRgbFromTextureTemplate = R"(
				<operation>
					<id>%id%</id>
					<returnType>vec3</returnType>
					<function>readRgbFromTexture</function>
					<arguments>
						<argument>%map%</argument>
						<argument>out2</argument>
					</arguments>
				</operation>)";

	static const char* readRFromTextureTemplate = R"(
				<operation>
					<id>%id%</id>
					<returnType>float</returnType>
					<function>readRFromTexture</function>
					<arguments>
						<argument>%map%</argument>
						<argument>out2</argument>
					</arguments>
				</operation>)";

	// Compose full template
	// First geometry part
	std::string materialStr;
	materialStr = R"(<?xml version="1.0" encoding="UTF-8" ?>)";
	materialStr += "\n<material>\n\t<programs>\n";
	if(dispTex.empty())
	{
		materialStr += simpleVertTemplate;
	}
	else
	{
		materialStr += tessVertTemplate;
	}

	materialStr += "\n";

	// Then fragment part
	materialStr += diffNormSpecFragTemplate;
	materialStr += "\n\t</programs>\t</material>";

	// Replace strings
	if(!dispTex.empty())
	{
		materialStr = replaceAllString(materialStr, "%dispMap%",
			m_texrpath + dispTex);
	}

	// Diffuse
	if(!diffTex.empty())
	{
		materialStr = replaceAllString(materialStr, "%diffuseColorInput%",
			R"(<input><type>sampler2D</type><name>uDiffuseColor</name><value>)"
			+ m_texrpath + diffTex
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%diffuseColorFunc%",
			readRgbFromTextureTemplate);

		materialStr = replaceAllString(materialStr, "%id%",
			"10");

		materialStr = replaceAllString(materialStr, "%map%",
			"uDiffuseColor");

		materialStr = replaceAllString(materialStr, "%diffuseColorArg%",
			"out10");
	}
	else
	{
		aiColor3D diffCol = {0.0, 0.0, 0.0};
		mtl.Get(AI_MATKEY_COLOR_DIFFUSE, diffCol);

		materialStr = replaceAllString(materialStr, "%diffuseColorInput%",
			R"(<input><type>vec3</type><name>uDiffuseColor</name><value>)"
			+ std::to_string(diffCol[0]) + " "
			+ std::to_string(diffCol[1]) + " "
			+ std::to_string(diffCol[2])
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%diffuseColorFunc%",
			"");

		materialStr = replaceAllString(materialStr, "%diffuseColorArg%",
			"uDiffuseColor");
	}

	// Normal
	if(!normTex.empty())
	{
		materialStr = replaceAllString(materialStr, "%normalInput%",
			R"(<input><type>sampler2D</type><name>uNormal</name><value>)"
			+ m_texrpath + normTex
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%normalFunc%",
				R"(
				<operation>
					<id>20</id>
					<returnType>vec3</returnType>
					<function>readNormalFromTexture</function>
					<arguments>
						<argument>out0</argument>
						<argument>out1</argument>
						<argument>uNormal</argument>
						<argument>out2</argument>
					</arguments>
				</operation>)");

		materialStr = replaceAllString(materialStr, "%normalArg%",
			"out20");
	}
	else
	{
		materialStr = replaceAllString(materialStr, "%normalInput%", " ");

		materialStr = replaceAllString(materialStr, "%normalFunc%", " ");

		materialStr = replaceAllString(materialStr, "%normalArg%", "out0");
	}

	// Specular
	if(!specColTex.empty())
	{
		materialStr = replaceAllString(materialStr, "%specularColorInput%",
			R"(<input><type>sampler2D</type><name>uSpecularColor</name><value>)"
			+ m_texrpath + specColTex
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%specularColorFunc%",
			readRgbFromTextureTemplate);

		materialStr = replaceAllString(materialStr, "%id%",
			"50");

		materialStr = replaceAllString(materialStr, "%map%",
			"uSpecularColor");

		materialStr = replaceAllString(materialStr, "%specularColorArg%",
			"out50");
	}
	else
	{
		aiColor3D specCol = {0.0, 0.0, 0.0};
		mtl.Get(AI_MATKEY_COLOR_SPECULAR, specCol);

		materialStr = replaceAllString(materialStr, "%specularColorInput%",
			R"(<input><type>vec3</type><name>uSpecularColor</name><value>)"
			+ std::to_string(specCol[0]) + " "
			+ std::to_string(specCol[1]) + " "
			+ std::to_string(specCol[2])
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%specularColorFunc%",
			"");

		materialStr = replaceAllString(materialStr, "%specularColorArg%",
			"uSpecularColor");
	}

	if(!shininessTex.empty())
	{
		materialStr = replaceAllString(materialStr, "%specularPowerInput%",
			R"(<input><type>sampler2D</type><name>uSpecularPower</name><value>)"
			+ m_texrpath + shininessTex
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%specularPowerValue%",
			m_texrpath + shininessTex);

		materialStr = replaceAllString(materialStr, "%specularPowerFunc%",
			readRFromTextureTemplate);

		materialStr = replaceAllString(materialStr, "%id%",
			"60");

		materialStr = replaceAllString(materialStr, "%map%",
			"uSpecularPower");

		materialStr = replaceAllString(materialStr, "%specularPowerArg%",
			"out60");
	}
	else
	{
		float shininess = 0.0;
		mtl.Get(AI_MATKEY_SHININESS, shininess);
		const float MAX_SHININESS = 511.0;
		shininess = std::min(MAX_SHININESS, shininess);
		if(shininess > MAX_SHININESS)
		{
			LOGW("Shininness exceeds %f", MAX_SHININESS);
		}

		shininess = shininess / MAX_SHININESS;

		materialStr = replaceAllString(materialStr, "%specularPowerInput%",
			R"(<input><type>float</type><name>uSpecularPower</name><value>)"
			+ std::to_string(shininess)
			+ R"(</value></input>)");

		materialStr = replaceAllString(materialStr, "%specularPowerFunc%",
			"");

		materialStr = replaceAllString(materialStr, "%specularPowerArg%",
			"uSpecularPower");
	}

	materialStr = replaceAllString(materialStr, "%maxSpecularPower%", " ");

	// Emission
	aiColor3D emissionCol = {0.0, 0.0, 0.0};
	mtl.Get(AI_MATKEY_COLOR_EMISSIVE, emissionCol);
	float emission = (emissionCol[0] + emissionCol[1] + emissionCol[2]) / 3.0;

	if(!emissiveTex.empty())
	{
		materialStr = replaceAllString(materialStr, "%emissionInput%",
			"<input><type>sampler2D</type><name>emissionTex</name><value>"
			+ m_texrpath + emissiveTex
			+ "</value></input>)\n"
			+ "\t\t\t\t<input><type>float</type><name>emission</name><value>"
			+ std::to_string(5.0)
			+ "</value><const>1</const></input>");

		std::string func = readRFromTextureTemplate;
		func = replaceAllString(func, "%id%", "71");
		func = replaceAllString(func, "%map%", "emissionTex");
		func += R"(
				<operation>
					<id>70</id>
					<returnType>float</returnType>
					<function>mul</function>
					<arguments>
						<argument>out71</argument>
						<argument>emission</argument>
					</arguments>
				</operation>)";

		materialStr = replaceAllString(materialStr, "%emissionFunc%", func);

		materialStr = replaceAllString(materialStr, "%map%", "emissionTex");

		materialStr = replaceAllString(materialStr, "%emissionArg%",
			"out70");
	}
	else
	{
		materialStr = replaceAllString(materialStr, "%emissionInput%",
			R"(<input><type>float</type><name>emission</name><value>)"
			+ std::to_string(emission)
			+ R"(</value><const>1</const></input>)");

		materialStr = replaceAllString(materialStr, "%emissionFunc%", "");

		materialStr = replaceAllString(materialStr, "%emissionArg%",
			"emission");
	}

	materialStr = replaceAllString(materialStr, "%instanced%",
		(instances > 1) ? "1" : "0");
	materialStr = replaceAllString(materialStr, "%arraySize%",
		std::to_string(roundUpInstancesCount(instances)));
	materialStr = replaceAllString(materialStr, "%diffuseMap%",
		m_texrpath + diffTex);

	// Subsurface
	materialStr = replaceAllString(materialStr, "%subsurfaceInput%",
		"<input><type>float</type><name>subsurface</name>"
		"<const>1</const><value>0.0</value></input>");
	materialStr = replaceAllString(materialStr, "%subsurfaceArg%",
		"subsurface");

	// Replace texture extensions with .anki
	materialStr = replaceAllString(materialStr, ".tga", ".ankitex");
	materialStr = replaceAllString(materialStr, ".png", ".ankitex");
	materialStr = replaceAllString(materialStr, ".jpg", ".ankitex");
	materialStr = replaceAllString(materialStr, ".jpeg", ".ankitex");

	// Open and write file
	std::fstream file;
	file.open(m_outputDirectory + name + ".ankimtl", std::ios::out);
	file << materialStr;
}
