#include "stdafx.h"
#include "AlembicPropertyUtils.h"
#include <sstream>
#include "AlembicMAXScript.h"


void createStringPropertyDisplayModifier(std::string modname, std::vector<std::pair<std::string, std::string>>& nameValuePairs)
{
   //Example usage:
   //
   //std::vector<std::pair<std::string, std::string>> nameValuePairs;
   //nameValuePairs.push_back(std::pair<std::string, std::string>("Prop1", "Prop1Val"));
   //nameValuePairs.push_back(std::pair<std::string, std::string>("Prop2", "Prop2Val"));
   //nameValuePairs.push_back(std::pair<std::string, std::string>("Prop3", "Prop3Val"));
   //nameValuePairs.push_back(std::pair<std::string, std::string>("Prop4", "Prop4Val"));
   //createPropertiesDisplayModifier(std::string("My Properties"), nameValuePairs);

   //the script assumes a single object is selected

   std::stringstream evalStream;
   evalStream<<"propModifier = EmptyModifier()"<<"\n";
   evalStream<<"propModifier.name = \""<<modname<<"\""<<"\n";
   evalStream<<"modCount = $.modifiers.count"<<"\n";
   evalStream<<"addmodifier $ propModifier before:modCount"<<"\n";
   evalStream<<"$.modifiers[\""<<modname<<"\"].enabled = false"<<"\n";

   evalStream<<"propAttribute = attributes propAttribute"<<"\n";
   evalStream<<"("<<"\n";

   evalStream<<"parameters propAttributePRM1 rollout:propAttributeRLT1"<<"\n";
   evalStream<<"("<<"\n";
   for(int i=0; i<nameValuePairs.size(); i++){
      std::string& name = nameValuePairs[i].first;
      std::string& val = nameValuePairs[i].second;
      evalStream<<name<<" type:#string ui:e"<<name<<" default:\""<<val<<"\""<<"\n";
   }
   evalStream<<")"<<"\n";
   
   evalStream<<"rollout propAttributeRLT1 \""<<modname<<"\""<<"\n";
   evalStream<<"("<<"\n";
   for(int i=0; i<nameValuePairs.size(); i++){
      std::string& name = nameValuePairs[i].first;
      //std::string& val = nameValuePairs[i].second;
      evalStream<<"edittext e"<<name<<" \""<<name<<"\" fieldWidth:140 labelOnTop:true"<<"\n";
   }
   evalStream<<")"<<"\n";
   evalStream<<")"<<"\n";


   evalStream<<"custattributes.add $.modifiers[\""<<modname<<"\"] propAttribute baseobject:false"<<"\n";

   ExecuteMAXScriptScript( EC_UTF8_to_TCHAR((char*)evalStream.str().c_str()) );
}

Modifier* createDisplayModifier(std::string modkey, std::string modname, std::vector<AbcProp>& props)
{
   ESS_PROFILE_FUNC();


   //the script assumes a single object is selected

   std::stringstream evalStream;
   evalStream<<"propModifier = EmptyModifier()"<<"\n";
   evalStream<<"propModifier.name = \""<<modkey<<"\""<<"\n";
   evalStream<<"modCount = $.modifiers.count"<<"\n";
   evalStream<<"addmodifier $ propModifier before:modCount"<<"\n";
   evalStream<<"$.modifiers[\""<<modkey<<"\"].enabled = false"<<"\n";

   evalStream<<"propAttribute = attributes propAttribute"<<"\n";
   evalStream<<"("<<"\n";

   evalStream<<"parameters propAttributePRM1 rollout:propAttributeRLT1"<<"\n";
   evalStream<<"("<<"\n";
   for(int i=0; i<props.size(); i++){
      std::string& name = props[i].name;
      std::string& val = props[i].displayVal;
      bool& bConstant = props[i].bConstant;

      const AbcA::DataType& datatype = props[i].propHeader.getDataType();
      const AbcA::MetaData& metadata = props[i].propHeader.getMetaData();

      if(datatype.getPod() == AbcA::kInt32POD && datatype.getExtent() == 1){
         evalStream<<name<<" type:#integer animatable:true ui:e"<<name<<" default:"<<val;
      }
      else if(datatype.getPod() == AbcA::kFloat32POD && datatype.getExtent() == 1){
         evalStream<<name<<" type:#float animatable:true ui:e"<<name<<" default:"<<val;
      }
      else if(datatype.getPod() == AbcA::kFloat32POD && datatype.getExtent() == 3){
         //if(metadata.get("interpretation") == std::string("rgb")){
         //   evalStream<<name<<" type:#color animatable:false ui:e"<<name<<" default:(["<<val<<"] as color)"; 
         //}
         //else{
            //evalStream<<name<<" type:#point3 animatable:false ui:e"<<name<<" ["<<val<<"]"; 
            evalStream<<name<<"x type:#float animatable:true ui:e"<<name<<"x default:300\n";
            evalStream<<name<<"y type:#float animatable:true ui:e"<<name<<"y default:300\n";
            evalStream<<name<<"z type:#float animatable:true ui:e"<<name<<"z default:300\n";
         //}
      }
      else{
         evalStream<<name<<" type:#string animatable:false ui:e"<<name<<" default:\""<<val<<"\"";
      }
      evalStream<<"\n";
   }
   evalStream<<")"<<"\n";
   
   evalStream<<"rollout propAttributeRLT1 \""<<modname<<"\""<<"\n";
   evalStream<<"("<<"\n";

   for(int i=0; i<props.size(); i++){
      std::string& name = props[i].name;
      bool& bConstant = props[i].bConstant;
      const AbcA::DataType& datatype = props[i].propHeader.getDataType();
      const AbcA::MetaData& metadata = props[i].propHeader.getMetaData();

      if(datatype.getPod() == AbcA::kInt32POD && datatype.getExtent() == 1){
         evalStream<<"label lbl"<<name<<" \""<<name<<"\" align:#left fieldWidth:140\n";
         evalStream<<"spinner e"<<name<<" \"\" type:#integer range:[-9999999,9999999,0] align:#left labelOnTop:true\n";
      }
      else if(datatype.getPod() == AbcA::kFloat32POD && datatype.getExtent() == 1){
         evalStream<<"label lbl"<<name<<" \""<<name<<"\" align:#left fieldWidth:140\n";
         evalStream<<"spinner e"<<name<<" \"\" type:#float range:[-9999999,9999999,0]align:#left labelOnTop:true\n";
      }
      else if(datatype.getPod() == AbcA::kFloat32POD && datatype.getExtent() == 3){

         evalStream<<"label lbl"<<name<<" \""<<name<<"\" align:#left fieldWidth:140\n";
         evalStream<<"spinner e"<<name<<"x\"\" across:3 type:#float range:[-9999999,9999999,0] fieldWidth:39 readOnly:true\n";
         evalStream<<"spinner e"<<name<<"y\"\" type:#float range:[-9999999,9999999,0] fieldWidth:39\n";
         evalStream<<"spinner e"<<name<<"z\"\" type:#float range:[-9999999,9999999,0] fieldWidth:39\n";
      }
      else{
         //TODO: better fit for large strings
         evalStream<<"edittext e"<<name<<" \" "<<name<<"\" fieldWidth:140 labelOnTop:true";
         if(bConstant) evalStream<<" readOnly:true";
      }

      evalStream<<"\n";
   }

   //evalStream<<"on propAttributePRM1 changed do\n";
   //evalStream<<"("<<"\n";

   //for(int i=0; i<props.size(); i++){
   //   std::string& name = props[i].name;

   //   const AbcA::DataType& datatype = props[i].propHeader.getDataType();
   //   const AbcA::MetaData& metadata = props[i].propHeader.getMetaData();

   //   evalStream<<"e"<<name<<".text = "<<name<<" as string";
   //   evalStream<<"\n";
   //}

   //evalStream<<")"<<"\n";
   evalStream<<")"<<"\n";
   evalStream<<")"<<"\n";


   evalStream<<"custattributes.add $.modifiers[\""<<modkey<<"\"] propAttribute baseobject:false"<<"\n";

   

   //ESS_LOG_WARNING(evalStream.str());

   evalStream<<"$.modifiers[\""<<modkey<<"\"]\n";
   FPValue fpVal;
   ExecuteMAXScriptScript( EC_UTF8_to_TCHAR((char*)evalStream.str().c_str()), 0, &fpVal);
   Modifier* pMod = (Modifier*)fpVal.r;
   return pMod;
}

void addFloatController(std::stringstream& evalStream, alembic_importoptions &options,
                        const std::string& modkey, std::string propName, const std::string& file, const std::string& identifier, 
                        std::string propertyID/*, const std::string& propComponent=std::string(""), const std::string& propInterp=std::string("")*/)
{
   //if(propComponent.length() > 0){
   //   propName += propComponent;
   //   propertyID += ".";
   //   propertyID += propInterp;
   //   propertyID += ".";
   //   propertyID += propComponent;
   //}

   std::string mkStr;
   if(!modkey.empty()){
      std::stringstream mkStream;
      mkStream<<".modifiers[\""<<modkey<<"\"]";
      mkStr = mkStream.str();
   }

   std::string timeControlName;
   if(options.pTimeControl){
      timeControlName = EC_MCHAR_to_UTF8( options.pTimeControl->GetObjectName() );
   }

   evalStream<<"$"<<mkStr<<"."<<propName<<".controller = AlembicFloatController()\n";
   evalStream<<"$"<<mkStr<<"."<<propName<<".controller.path = \""<<file<<"\"\n";
   evalStream<<"$"<<mkStr<<"."<<propName<<".controller.identifier = \""<<identifier<<"\"\n";
   evalStream<<"$"<<mkStr<<"."<<propName<<".controller.property = \""<<propertyID<<"\"\n";
   


   evalStream<<"$"<<mkStr<<"."<<propName<<".controller.time.controller = float_expression()\n";
   if(options.loadTimeControl){
      evalStream<<"$"<<mkStr<<"."<<propName<<".controller.time.controller.AddScalarTarget \"current\" $'"<<timeControlName<<"'.time.controller\n";
      evalStream<<"$"<<mkStr<<"."<<propName<<".controller.time.controller.setExpression \"current\"\n";
   }
   else{
      evalStream<<"$"<<mkStr<<"."<<"$.modifiers[\""<<modkey<<"\"]."<<propName<<".controller.time.controller.setExpression \"S\"\n";
   }

}



void addControllersToModifier(const std::string& modkey, const std::string& modname, std::vector<AbcProp>& props, 
                              const std::string& target, const std::string& type,
                              const std::string& file, const std::string& identifier, alembic_importoptions &options)
{
   ESS_PROFILE_FUNC();

   //the script assumes a single object is selected

   std::stringstream evalStream;

   for(int i=0; i<props.size(); i++){
      std::string propName = props[i].name;
      std::string& val = props[i].displayVal;
      bool& bConstant = props[i].bConstant;

      const AbcA::DataType& datatype = props[i].propHeader.getDataType();
      const AbcA::MetaData& metadata = props[i].propHeader.getMetaData();

      if(datatype.getPod() == AbcA::kFloat32POD){

         std::stringstream propStream;
         propStream<<target<<"."<<type<<"."<<propName;
         if(datatype.getExtent() == 1){
            addFloatController(evalStream, options, modkey, propName, file, identifier, propStream.str());
         }
         else if(datatype.getExtent() == 3){

            std::stringstream xStream, yStream, zStream;

            xStream<<propStream.str()<<"."<<metadata.get("interpretation")<<".x";
            yStream<<propStream.str()<<"."<<metadata.get("interpretation")<<".y";
            zStream<<propStream.str()<<"."<<metadata.get("interpretation")<<".z";

            addFloatController(evalStream, options, modkey, propName + std::string("x"), file, identifier, xStream.str());
            addFloatController(evalStream, options, modkey, propName + std::string("y"), file, identifier, yStream.str());
            addFloatController(evalStream, options, modkey, propName + std::string("z"), file, identifier, zStream.str());
         }
      }
      else{
         
      }
      evalStream<<"\n";
   }

  
   //ESS_LOG_WARNING(evalStream.str());

   ExecuteMAXScriptScript( EC_UTF8_to_TCHAR((char*)evalStream.str().c_str()));
}



template<class PT, class FT> bool readPropExt1(const Abc::ICompoundProperty& prop, const AbcA::PropertyHeader& pheader, std::string& val, bool& isConstant)
{
   if(PT::matches(pheader))
   {
      PT aProp(prop, pheader.getName());

      FT val1;
      aProp.get(val1);

      std::stringstream valStream;
      valStream<<val1;
      val = valStream.str();

      isConstant = aProp.isConstant();

      return true;
   }
   return false;
}

template<class PT, class FT> bool readPropExt3(const Abc::ICompoundProperty& prop, const AbcA::PropertyHeader& pheader, std::string& val, bool& isConstant)
{
   if(PT::matches(pheader))
   {
      PT aProp(prop, pheader.getName());

      FT val3;
      aProp.get(val3);

      std::stringstream valStream;
      valStream<<val3.x<<","<<val3.y<<","<<val3.z;
      val = valStream.str();

      return true;
   }
   return false;
}

bool sortFunc(AbcProp p1, AbcProp p2) { return p1.sortId > p2.sortId; }

char* getPodStr(AbcA::PlainOldDataType pod)
{
    if(pod == AbcA::kBooleanPOD) return "kBooleanPOD";
    if(pod == AbcA::kUint8POD) return "kUint8POD";
    if(pod == AbcA::kInt8POD) return "kInt8POD";
    if(pod == AbcA::kUint16POD) return "kUint16POD";
    if(pod == AbcA::kInt16POD) return "kInt16POD";
    if(pod == AbcA::kUint32POD) return "kUint32POD";
    if(pod == AbcA::kInt32POD) return "kInt32POD";
    if(pod == AbcA::kUint64POD) return "kUint64POD";
    if(pod == AbcA::kInt64POD) return "kInt64POD";
    if(pod == AbcA::kFloat16POD) return "kFloat16POD";
    if(pod == AbcA::kFloat32POD) return "kFloat32POD";
    if(pod == AbcA::kFloat64POD) return "kFloat64POD";
    if(pod == AbcA::kStringPOD) return "kStringPOD";
    if(pod == AbcA::kWstringPOD) return "kWstringPOD";
    if(pod == AbcA::kNumPlainOldDataTypes) return "kNumPlainOldDataTypes";
    //if(pod == AbcA::kUnknownPOD)  
    return "kUnknownPOD";
}

void readInputProperties( Abc::ICompoundProperty prop, std::vector<AbcProp>& props )
{
   if(!prop){
      return;
   }

   for(size_t i=0; i<prop.getNumProperties(); i++){
      AbcA::PropertyHeader pheader = prop.getPropertyHeader(i);
      AbcA::PropertyType propType = pheader.getPropertyType();

      if( propType == AbcA::kCompoundProperty ){
         //printInputProperties(Abc::ICompoundProperty(prop, pheader.getName()));
         ESS_LOG_WARNING("Unsupported compound property: "<<pheader.getName());
      }
      else if( propType == AbcA::kScalarProperty ){

         //ESS_LOG_WARNING("Scaler property: "<<pheader.getName());
         //

         std::string displayVal;
         bool bConstant = true;
         int sortId = 0;

         if(Abc::IBoolProperty::matches(pheader)){

            //I need to know the name and type only if animated; an appropriate controller will handle reading the data.
            //If not animated, the value will set directly on the light and/or display modifier

            Abc::IBoolProperty boolProp(prop, pheader.getName());
            /*if(boolProp.isConstant()){*/
               AbcU::bool_t bVal = false;
               boolProp.get(bVal);
               if(bVal == true) displayVal = "true";
               else displayVal = "false";
            //}
            //else{
            //  
            //}

         }
         else if(readPropExt1<Abc::IInt32Property, int>(prop, pheader, displayVal, bConstant));
         else if(readPropExt1<Abc::IFloatProperty, float>(prop, pheader, displayVal, bConstant));
         else if(readPropExt3<Abc::IC3fProperty, Abc::C3f>(prop, pheader, displayVal, bConstant));
         else if(readPropExt3<Abc::IV3fProperty, Abc::V3f>(prop, pheader, displayVal, bConstant));
         else if(readPropExt3<Abc::IN3fProperty, Abc::N3f>(prop, pheader, displayVal, bConstant));
         else if(Abc::IStringProperty::matches(pheader)){
            
            Abc::IStringProperty stringProp(prop, pheader.getName());
            stringProp.get(displayVal);
            sortId = 1000000000;
         }
         else{
      //   Abc::PropertyHeader propHeader = props.getPropertyHeader(i);
      //   AbcA::PropertyType propType = propHeader.getPropertyType();

            ESS_LOG_WARNING("Unsupported property, propName: "<<pheader.getName()<<", pod: "<<getPodStr(pheader.getDataType().getPod()) \
             <<", extent: "<<(int)pheader.getDataType().getExtent()<<", interpretation: "<<pheader.getMetaData().get("interpretation"));

         }

         props.push_back(AbcProp(pheader.getName(), displayVal, pheader, bConstant, sortId));
      }
      else if( propType == AbcA::kArrayProperty ){
         //ESS_LOG_WARNING("Unsupported array property: "<<pheader.getName());
         ESS_LOG_WARNING("Unsupported array property, propName: "<<pheader.getName()<<", pod: "<<getPodStr(pheader.getDataType().getPod()) \
         <<", extent: "<<(int)pheader.getDataType().getExtent()<<", interpretation: "<<pheader.getMetaData().get("interpretation"));
      }
      else{
         ESS_LOG_WARNING("Unsupported input property: "<<pheader.getName());
      }

   }
}
