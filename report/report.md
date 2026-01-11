# Introduction

This work is carried out within the framework of the **EIT Digital** program and the course **High‑Performance Computing for Data Science**. The objective of the project is to conduct an in‑depth analysis of an optimization algorithm, to design a **sequential implementation**, and subsequently to develop a **parallelized version** suitable for **HPC environments**.

We chose to study the **Bat Algorithm (BA)**, a bio‑inspired metaheuristic proposed by **Xin‑She Yang (2010)**, whose mechanism is based on a simplified mathematical modeling of the **echolocation behavior of microbats**. The goal of this first phase is to thoroughly understand the internal mechanisms of the algorithm, its parameters, and the way it explores the search space in order to solve **continuous optimization problems**.

Continuous optimization problems arise in many scientific and industrial domains and consist in finding, among all possible configurations, the one that minimizes or maximizes an objective function. As highlighted by **Nocedal and Wright** in *Numerical Optimization*, such problems can be particularly challenging when the objective function is **non‑convex**, **non‑differentiable**, **noisy**, or when the **dimensionality of the problem increases**. In these cases, gradient‑based methods often become ineffective.

A comprehensive review by **Rios and Sahinidis (2013)** further emphasizes that when derivatives are unavailable or unreliable, classical optimization methods are “*of little or no use*”, and that their performance deteriorates rapidly as the problem dimension grows. These limitations motivate the use of **derivative‑free** and **metaheuristic** approaches.

As a stochastic algorithm, the Bat Algorithm incorporates randomness to efficiently explore the search space. Its design explicitly balances **global exploration** and **local exploitation**, making it well‑suited for highly nonlinear objective functions and multimodal landscapes with multiple local optima.

---

# Principles of the Bat Algorithm

The Bat Algorithm is inspired by the echolocation mechanism used by microbats to navigate and hunt in complete darkness. In nature, bats emit ultrasonic pulses and analyze the returning echoes to estimate the distance, direction, size, and motion of nearby objects. As they approach a target, they adapt the **frequency**, **loudness**, and **pulse emission rate** of these signals.

Yang translates these biological observations into algorithmic concepts. Each bat is modeled as a candidate solution characterized by:

* a **position** $x_i$ in the search space,
* a **velocity** $v_i$,
* a **frequency** $f_i$ controlling the scale of movement,
* a **loudness** $A_i$ governing solution acceptance,
* a **pulse rate** $r_i$ controlling the switch between exploration and exploitation.

At each iteration, these parameters are updated stochastically, simulating the collective behavior of a bat colony progressively converging toward an optimal region.

---

# Echolocation Model and Biological Motivation

Microbats emit ultrasonic pulses with frequencies typically ranging from **25 kHz to 150 kHz**, each pulse lasting only a few milliseconds. During the search phase, bats emit around **10–20 pulses per second**, increasing up to **200 pulses per second** as they approach prey. Pulse loudness may reach **110 dB**, but decreases as the bat nears its target to improve precision and avoid sensory saturation.

From the reflected echoes, bats infer:

* **distance**, via time‑of‑flight measurement,
* **direction**, via interaural time differences,
* **target characteristics**, via echo intensity and Doppler effects.

The distance estimation follows the classical acoustic relation:

$$
\text{distance} = v \cdot \frac{t}{2},
$$

where $v$ is the speed of sound and $t$ is the round‑trip travel time of the signal.

This remarkable sensing capability motivates the abstraction used in the Bat Algorithm, where objective‑function evaluations play the role of echo analysis.

---

# Mathematical Formulation of the Bat Algorithm

Following Yang (2010), the Bat Algorithm is based on three idealized assumptions:

1. Each bat can evaluate the quality of its current position through the objective function.
2. Bats adapt their frequency, velocity, and pulse rate depending on their proximity to promising solutions.
3. Loudness decreases while pulse rate increases over time, modeling a transition from exploration to exploitation.

The **global update** equations are:

$$
f_i = f_{\min} + (f_{\max} - f_{\min}) , \beta, \quad \beta \sim U(0,1),
$$

$$
v_i^{t+1} = v_i^t + (x^* - x_i^t) , f_i,
$$

$$
x_i^{t+1} = x_i^t + v_i^{t+1},
$$

where $x^*$ denotes the current best solution.

The **local search** mechanism is defined as:

$$
x_{\text{new}} = x_{\text{best}} + \sigma , \varepsilon_t , A^{(t)},
$$

with $\varepsilon_t \sim \mathcal{N}(0,1)$ and $\sigma$ a scaling parameter.

The adaptive behavior of loudness and pulse rate follows:

$$
A_i^{t+1} = \alpha A_i^t, \quad r_i^{t+1} = r_{0i} \bigl(1 - e^{-\gamma t}\bigr),
$$

ensuring a gradual shift from exploration to exploitation.

---

# Algorithmic Structure

**Algorithm: Bat Algorithm**

**Input:** Objective function $f(x)$
**Output:** Best solution found

1. Initialize a population of $n$ bats with positions $x_i$ and velocities $v_i$.
2. Initialize frequencies $f_i$, loudness $A_i$, and pulse rates $r_i$.
3. While $t < T_{\max}$:

   * Update frequencies, velocities, and positions.
   * With probability $1 - r_i$, generate a local solution around the current best.
   * Evaluate candidate solutions and apply acceptance criteria based on $A_i$.
   * Update loudness and pulse rate if improvement occurs.
   * Update the global best solution $x^*$.
4. Return $x^*$.

---

# Conceptual Limitations and Critical Analysis

A careful examination of Yang’s original formulation reveals several **conceptual inconsistencies**, particularly regarding the interpretation of the **frequency** $f_i$ and the **pulse rate** $r_i$.

* In biological systems, **higher frequencies correspond to fine‑grained localization**, whereas in the algorithm, higher $f_i$ values produce **larger displacements**, which is more characteristic of global exploration.
* The condition `rand > r_i` used to trigger local search appears counter‑intuitive if $r_i$ is interpreted as increasing near the optimum.

Moreover, an inconsistency exists between the velocity update equation in the original paper and the MATLAB implementation provided by Yang, leading to a **sign compensation effect** between frequency and velocity updates. This ambiguity complicates the theoretical interpretation of the algorithm and motivates careful implementation choices.

---

# Sequential Implementation Choices

Our sequential C implementation is inspired by:

* Yang’s original 2010 paper,
* the MATLAB reference implementation from *Nature‑Inspired Optimization Algorithms* (2014),
* and the clarified equations presented in the 2020 edition.

We explicitly correct the velocity update to ensure attraction toward the best solution, while preserving the original pulse‑rate condition. Two candidate solutions (global and local) are evaluated separately at each iteration, improving algorithmic clarity compared to the MATLAB version, which overwrites the global solution when local search is triggered.

Parameter choices (population size, frequency bounds, loudness, pulse rate, domain limits) strictly follow Yang’s recommendations, with minor adaptations justified by the characteristics of the chosen objective function.

---

*To do:*

* Refine and standardize academic citations.
* Insert and document the sequential C implementation.

-----

# Parallel Design

#### Analysis of Parallelism in the Bat Algorithm

The **Bat Algorithm** is a population-based metaheuristic in which a set of individuals (bats) evolve simultaneously within the search space.  
At each iteration, every individual updates its position, velocity, and internal parameters, and then evaluates the objective function associated with its current position.

Analysis of the sequential algorithm shows that most of these operations are **independent for each individual**. In particular, the following steps exhibit no direct dependencies between bats:

- updating the frequency, velocity, and position;
- evaluating the objective function;
- updating the *loudness* and *pulse rate* parameters.

These operations constitute the most computationally expensive part of the algorithm and naturally lend themselves to **data parallelism**, where different individuals can be processed simultaneously on distinct computing units.

However, the algorithm introduces a **global dependency** through the best solution found so far, denoted \( x_{\text{best}} \). This information is used by all individuals to guide local search. Consequently, although the algorithm is largely parallelizable, a **global synchronization** is required to ensure consistency of this information across the different computing units.

#### Master–Worker Model

A first parallelization strategy for the Bat Algorithm is based on a **master–worker model**, as proposed by Noor et al. in *[Performance of Parallel Distributed Bat Algorithm using MPI on a PC Cluster](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3559955)*. From our perspective, this approach appears to be the most natural solution for parallelizing the Bat Algorithm in a distributed MPI environment.

##### General Principle

In this model, the global bat population is divided among several worker processes, while a distinct process plays the role of the master. Each worker locally executes the steps of the sequential Bat Algorithm on a sub-population, while the master is responsible for global synchronization and for selecting the best solution across the entire population.

The authors describe this behavior using a pseudo-code entitled **Parallel-Distributed Bat Algorithm – Master–Worker (PDBA-MW)**, which highlights the respective responsibilities of the workers and the master.

##### Parallel-Distributed Bat Algorithm – Master–Worker (PDBA-MW)

**Initialization**
- Initialize `every_send_time` to 50

**Main Loop**
- For `iteration = 1` to `Max_iteration`:

  - **If (Worker Node)** then:
    - For `i = 1` to `newpopsize`:
      - *Note*: `newpopsize = N / total_workers`
      - Perform Steps 1 to 5 of the sequential algorithm
    - End For

    - If (`iteration == every_send_time`) then:
      - Each `worker_k` sends its best candidate solution and corresponding fitness to the Master
      - Receive the best solution and fitness from the Master node
    - End If

  - **Else if (Master Node)** then:
    - For `k = 1` to `Total_Workers`:
      1. Receive `best_k` from each worker
      2. Compare and choose the best among all received solutions
      3. Send the best solution to each worker
    - End For

- End For

**Termination Condition**
- The process is terminated when the desired accuracy is achieved  
  or when the maximum number of iterations has been reached.

##### Initialization and Communication Parameterization

The pseudo-code introduces a parameter `every_send_time`, initialized to a fixed value (for example, 50), which controls the communication frequency between the workers and the master. This parameter aims to space out synchronizations between processes in order to limit overly frequent communications that could increase the overall execution cost.

##### Parallel Processing on the Worker Side

At each iteration, each worker executes the Bat Algorithm on a sub-population of size  
`newpopsize = N / total_workers`, where `N` denotes the total population size. As indicated in the pseudo-code, each worker locally applies the main steps of the sequential algorithm (updating positions and velocities, generating candidate solutions, evaluating the objective function, and updating internal parameters).

The operations performed during this phase are executed locally by each worker, without inter-process communication. The independent processing of sub-populations thus allows the computation to be distributed across multiple cluster nodes.

##### Periodic Synchronization with the Master

When the condition `iteration == every_send_time` is satisfied, each worker identifies its best local solution and sends it to the master process, along with the corresponding fitness value. In the implementation proposed by the authors, this step relies on point-to-point communications between the workers and the master, using the MPI `Send/Recv` primitives.

The master then receives the local best solutions sent by the workers, compares them, and extracts the global best solution. This solution is subsequently broadcast to all workers and used as \( x_{\text{best}} \) in the following iterations of the algorithm.

##### Critical Analysis of the Model

The master–worker model introduces a clear and easy-to-implement organization, but it relies on a **centralized global synchronization** around the master process. At each synchronization phase, the master must receive the local best solutions from all workers, perform a global comparison, and redistribute the best solution \( x_{\text{best}} \) to all processes. This organization implies that the communication volume handled by the master increases **linearly with the number of workers**.

When the number of workers becomes large, the master process can thus become a **potential bottleneck**, particularly due to the use of blocking MPI communications such as `Send` and `Recv`, as indicated by the authors. In this context, the time spent in communication may gradually limit the gains obtained from parallelization, even though this situation is not explicitly studied in the presented experiments.

The experimental results nevertheless show an improvement in execution time and an **increasing speed-up** as the cluster size grows, especially for large population sizes. However, the efficiency curves also indicate a gradual decrease in efficiency as the number of nodes increases, which can be interpreted as a combined effect of communication overhead and the load imposed on the master process.

Moreover, the choice of a fixed value for the `every_send_time` parameter reduces synchronization frequency but introduces a **parameter dependency** whose impact on algorithm convergence is not analyzed in detail. Excessively spaced synchronizations may delay the dissemination of \( x_{\text{best}} \), while overly frequent synchronizations may amplify communication costs, particularly in a distributed setting.

Thus, the master–worker model enables a straightforward parallelization of the Bat Algorithm and ensures dissemination of the global best solution to all processes. However, this approach relies on centralized synchronization that may become limiting as the number of workers increases. The communication load borne by the master process then grows with cluster size, potentially affecting the overall scalability of the algorithm.

These limitations have led to the investigation of alternative strategies aimed at reducing communication centralization. Among them, models based on largely independent sub-populations offer a more distributed organization, potentially reducing synchronization costs and improving scalability on larger parallel architectures.

#### Independent Sub-Population Model (Island Model)

The independent sub-population model, also known as the *Island Model*, is based on dividing the global population into several groups that evolve autonomously. This strategy is notably adopted by Tsai et al., who propose a parallelized version of the Bat Algorithm incorporating a periodic communication strategy between sub-populations.

##### Steps of the Sub-Population Model

##### Initialization

The initial bat population is generated and then divided into \( G \) sub-populations. Each subgroup is independently initialized using the Bat Algorithm.

A set of iterations \( R \) is defined to determine when the communication strategy will be applied. The position of a bat is denoted \( X_{ij}^t \), where \( i \) refers to the individual within subgroup \( j \), and \( t \) denotes the current iteration. The algorithm starts with \( t = 1 \).

##### Evaluation

At each iteration, the objective function \( f(X_{ij}^t) \) is evaluated for all bats in each sub-population. This step is performed locally, without any information exchange between groups.

##### Update

Bat velocities and positions are updated using the equations of the original Bat Algorithm. Each sub-population evolves independently and maintains its own local best solution, without direct knowledge of the solutions found by other groups.

##### Communication Strategy

The communication strategy is activated only at specific iterations defined by the set  
\( R = \{ R_1, 2R_1, 3R_1, \dots \} \).

When a communication iteration is reached, each subgroup selects the \( k \) best bats according to their fitness values. These solutions are then copied to the neighboring subgroup \( g_{(p+1) \bmod G} \), where they replace the same number of worst-performing solutions.

This mechanism allows a gradual diffusion of information between sub-populations, without global synchronization or a centralized process.

##### Termination

The previous steps are repeated until a stopping criterion is met, such as a maximum number of iterations or achieving a target value of the objective function. The best value obtained and its corresponding position are then recorded as the final solution.

##### Limitations of the Independent Sub-Population Model

The independent sub-population model proposed by Tsai et al. reduces communication frequency and avoids centralized global synchronization. However, this organization introduces certain limitations from the perspective of algorithmic dynamics.

In particular, the global best solution is not continuously shared among all individuals. Each sub-population evolves with its own local best solution, and information from other groups is only integrated periodically through migrations. This delayed diffusion may slow convergence, especially when some sub-populations explore less promising regions of the search space.

Furthermore, this strategy relies on several additional parameters, such as the number of sub-populations, communication frequency, and the number of migrated solutions. The choice of these parameters strongly influences the balance between exploration and exploitation, without their impact being thoroughly analyzed.

Thus, while this model promotes diversity and scalability, it deviates from the behavior of the sequential Bat Algorithm and introduces significant trade-offs between convergence quality, information diffusion speed, and parameterization complexity.

In the remainder of this work, we propose an approach aimed at leveraging parallelization while limiting modifications to the original functioning of the algorithm.
