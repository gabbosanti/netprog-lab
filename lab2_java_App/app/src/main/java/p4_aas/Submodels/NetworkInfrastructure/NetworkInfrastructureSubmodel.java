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

package p4_aas.Submodels.NetworkInfrastructure;

import org.eclipse.basyx.submodel.metamodel.map.Submodel;
import org.eclipse.basyx.submodel.metamodel.map.qualifier.LangStrings;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.File;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.valuetype.ValueType;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.operation.Operation;
import p4_aas.NetworkController.Utils.ApiEnum;
import p4_aas.Submodels.AbstractSubmodel;
import java.util.List;
import java.util.Map;

public class NetworkInfrastructureSubmodel extends AbstractSubmodel {

    private NetworkInfrastructureLambda lambdaProvider;

    public NetworkInfrastructureSubmodel() {
        super();
        this.lambdaProvider = new NetworkInfrastructureLambda(this.getNetworkController());
    }

    @Override
    public List<Submodel> createSubmodel() {
		Submodel swPrograms = new Submodel();
        swPrograms.setIdShort("SwPrograms");

        swPrograms.addSubmodelElement(getRunningProgram());
        swPrograms.addSubmodelElement(setProgram());
        swPrograms.addSubmodelElement(description());
        swPrograms.addSubmodelElement(getMessageCounters());

		return List.of(swPrograms);
	}

    private File description() {
        File file = new File();
        file.setMimeType("image/jpg");
        file.setIdShort("Description");
        file.setValue("https://raw.githubusercontent.com/ricbacca/KatharaP4AAS/main/docs/P4AAS.jpg");

        return file;
    }

    /**
     * Operation into SwPrograms submodel.
     * Input: Switch numer (1 or 2).
     * Output: current P4 program executed by the P4 Switch.
     */
    private Operation getRunningProgram() {
        Operation currentProgram = new Operation("CurrentProgram");

        currentProgram.setInputVariables(getUtils().getCustomInputVariables(Map.of("Switch", ValueType.Integer)));
        currentProgram.setOutputVariables(getUtils().getOperationVariables(1, "output"));
        currentProgram.setWrappedInvokable(lambdaProvider.getRequest());

        return currentProgram;
    }

    /**
     * Input: new program name, to be executed by the specified Switch.
     * @return Operation to set a new program in the specified Switch.
     */
    private Operation setProgram() {
        Operation setProgram = new Operation("SetProgram");

        String description = "Programs:" +
        this.getNetworkController().getRequest(ApiEnum.AVAILABLEPROGRAMS_SW1.url);
        
        setProgram.setDescription(new LangStrings("English", description));
        setProgram.setInputVariables(getUtils().getCustomInputVariables(Map.of(
            "Switch", ValueType.Integer,
            "Program", ValueType.String,
            "Host", ValueType.String
        )));

        setProgram.setWrappedInvokable(lambdaProvider.changeProgram());

        return setProgram;
    }

    /**
     * Operation into SwPrograms submodel.
     * Input: Switch numer (1 or 2).
     * Input: Switch port
     * Output: current message and bytes transferred in that port
     */
    private Operation getMessageCounters() {
        Operation getCounters = new Operation("GetCounters");

        getCounters.setInputVariables(getUtils().getCustomInputVariables(Map.of("Switch", ValueType.Integer, "Ports", ValueType.String)));
        getCounters.setOutputVariables(getUtils().getOperationVariables(1, "output"));
        getCounters.setWrappedInvokable(lambdaProvider.getCounters());

        return getCounters;
    }
}