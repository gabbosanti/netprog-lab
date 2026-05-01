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

package p4_aas;

/**
 * To manage System properties, used to mantain info throughout the app.
 */
public class StaticProperties {
    public static final String REGISTRY_POLLING_IP = "http://100.0.2.1:4000/registry/api/v1/registry";
    public static final String REGISTRYPATH = "http://100.0.2.1:4000/registry/";

    public static final String YAML_FILE_PATH = "net-config.yml";

    public static final String SW1_IP = "http://100.0.1.2:3333";
    public static final String SW2_IP = "http://100.0.1.4:4444";

    public static final int NetworkInfrastructurePort = 6001;
    public static final int NetworkControlPlanePort = 6002;
    public static final int MachineOnePort = 6003;
    public static final int MachineTwoPort = 6004;
    public static final int MachineThreePort = 6005;

    public static final String Host1 = "10.0.1.1";
    public static final String Host2 = "10.0.1.2";
    public static final String Host3 = "10.0.2.1";
    public static final String Host4 = "10.0.2.2";
    public static final String Host5 = "10.0.3.1";
    public static final String Host6 = "10.0.3.2";
}