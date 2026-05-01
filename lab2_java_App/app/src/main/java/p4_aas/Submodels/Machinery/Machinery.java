// Copyright 2023 riccardo.bacca@studio.unibo.it
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package p4_aas.Submodels.Machinery;

import java.util.List;

import org.eclipse.basyx.submodel.metamodel.api.qualifier.haskind.ModelingKind;
import org.eclipse.basyx.submodel.metamodel.map.Submodel;
import org.eclipse.basyx.submodel.metamodel.map.qualifier.LangStrings;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.File;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.AASLambdaPropertyHelper;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.Property;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.valuetype.ValueType;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.operation.Operation;

import p4_aas.Submodels.AbstractSubmodel;

import java.util.Map;

public class Machinery extends AbstractSubmodel {
    private String manual;
    private String swVersion;
    private List<String> hosts;
    private MachineryLambda lambdaProvider;

    public Machinery(String manual, String version, List<String> hosts) {
        super();
        this.manual = manual;
        this.swVersion = version;
        this.hosts = hosts;
        this.lambdaProvider = new MachineryLambda();
    }

    @Override
    public List<Submodel> createSubmodel() {
        Submodel basicInfo = new Submodel();
        basicInfo.setDescription(this.getDescription());
        basicInfo.setIdShort("Info&Manuals");
        basicInfo.addSubmodelElement(setManual());
        basicInfo.addSubmodelElement(setVersion());

        Submodel operations = new Submodel();
        operations.setIdShort("Operations");
        operations.addSubmodelElement(this.pingMachinery());

        return List.of(basicInfo, operations);
    }

    private LangStrings getDescription() {
        return new LangStrings("English", "This machine is composed by host: " + hosts);
    }

    private File setManual() {
        File file = new File();
        file.setMimeType("File");
        file.setIdShort("Manuals");
        file.setValue(manual);

        return file;
    }

    private Property setVersion() {
        Property version = new Property("Version", swVersion);
        version.setKind(ModelingKind.INSTANCE);

        AASLambdaPropertyHelper.setLambdaValue(version, () -> swVersion, (newVersion) -> {
            this.swVersion = (String) newVersion;
            version.setValue(newVersion);
        });
        return version;
    }

    private Operation pingMachinery() {
        Operation pingMachinery = new Operation("PingMachinery");
        pingMachinery.setDescription(this.getDescription());
        pingMachinery.setInputVariables(getUtils().getCustomInputVariables(Map.of(
            "From", ValueType.String,
            "To", ValueType.String
        )));

        pingMachinery.setOutputVariables(getUtils().getOperationVariables(1, "Output"));
        pingMachinery.setWrappedInvokable(lambdaProvider.pingMachinery(hosts));

        return pingMachinery;
    }
}