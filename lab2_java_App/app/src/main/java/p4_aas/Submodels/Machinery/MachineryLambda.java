package p4_aas.Submodels.Machinery;

// Copyright 2024 riccardo.bacca@studio.unibo.it
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

import java.util.List;
import java.util.Map;
import java.util.function.Function;

import org.eclipse.basyx.submodel.metamodel.map.submodelelement.SubmodelElement;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.Property;

public class MachineryLambda {
    private PingTest pingHost;

    public MachineryLambda() {
        this.pingHost = new PingTest();
    }

    /**
     * @param machineHosts used to make Ping requests from
     * @return Ping test results from Speicifc IP (on this machine) to given IP from WebUI
     */
    public Function<Map<String, SubmodelElement>, SubmodelElement[]> pingMachinery(List<String> machineHosts) {
        return (args) -> {
            String pingFrom = ((String) args.get("From").getValue());
            String pingDst = ((String) args.get("To").getValue());

            if (machineHosts.contains(pingFrom)) {
                return new SubmodelElement[] {
                    new Property("Result: " + this.pingHost.pingTest(pingDst, pingFrom))
                };
            } else {
                return new SubmodelElement[] {
                    new Property("Selected 'from' host, must be in this machine !")
                };
            }
        };
    }
}