import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'dual_arm_simulation'

setup(
    name=package_name,
    version='0.1.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # Install launch files
        (os.path.join('share', package_name, 'launch'),
            glob(os.path.join('launch', '*.launch.py'))),
        # ── Install ALL MuJoCo scene files into share ──────────────────────
        # This is what makes get_package_share_directory() find the XMLs.
        # We walk the mujoco_menagerie tree and replicate it under share/.
        *[
            (
                os.path.join('share', package_name, dirpath),
                [os.path.join(dirpath, f) for f in filenames]
            )
            for dirpath, _, filenames in os.walk('mujoco_menagerie')
            if filenames
        ],
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Group 3',
    maintainer_email='group3@example.com',
    description='MuJoCo dual Panda simulation node',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'dual_arm_sim_node = dual_arm_simulation.dual_arm_sim_node:main',
        ],
    },
)