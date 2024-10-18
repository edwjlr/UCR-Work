import json
import matplotlib.pyplot as plt
import networkx as nx

# Load the origins to domains data
with open('cookieOriginsGraph.json', 'r') as file:
    origin_to_domains = json.load(file)

# Initialize a new graph
G = nx.Graph()

# Add nodes and edges to the graph
for origin, domains in origin_to_domains.items():
    for domain in domains:
        G.add_edge(origin, domain)  # Connect origin URL to domain

# Drawing the graph
plt.figure(figsize=(12, 8))
pos = nx.spring_layout(G)  # Positions for all nodes
nx.draw_networkx_nodes(G, pos, node_size=700, node_color="skyblue", alpha=0.6)
nx.draw_networkx_edges(G, pos, width=1, alpha=0.7, edge_color="gray")
nx.draw_networkx_labels(G, pos, font_size=10)
plt.title('Website Cookies Interconnection Graph')
plt.axis('off')  # Turn off the axis
plt.show()
