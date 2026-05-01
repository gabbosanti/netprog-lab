/* Copyright 2022 P4lang Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Example custom extern function.
extern void sha256_hash_256<T, B>(inout T a, in B b);
extern void sha256_hash_512<T, B, C>(inout T a, in B b, in C c);
extern void sha256_hash_1024<T, B, C, D, E, F>(inout T a, in B b, in C c, in D d, in E e, in F f);
extern void verify_hash_equals<A, B, C>(inout A a, in B b, in C c);


extern void Encrypt<T, B, K, L, N, M, O, P, Q, R, S, U>(in T a, inout B b, in K k1, in L k2, in N k3, in M k4, in O k5, in P k6, in Q k7, in R k8, in S len, in U seqNo);
extern void Decrypt<T, B, K, L, N, M, O, P, Q, R, S, W, Z, Y>(in T a, inout B b, in K k1, in L k2, in N k3, in M k4, in O k5, in P k6, in Q k7, in R k8, in S len, in W sha, in Z seqNo, inout Y shaCalculated);
